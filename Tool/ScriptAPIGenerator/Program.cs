using System.Text.Json;

namespace ScriptAPIGenerator;

internal static class Program
{
    private static int Main(string[] args)
    {
        if (!TryGetOptions(args, out GenerationOptions? options))
        {
            Console.Error.WriteLine(
                "Usage: ScriptAPIGenerator " +
                "--input <RuntimeAPI.json path> " +
                "--csharp-output <RuntimeAPI.g.cs path> " +
                "--cpp-output <RuntimeAPI.gen.h path>");

            return 1;
        }

        if (!File.Exists(options.InputPath))
        {
            Console.Error.WriteLine(
                $"Input file was not found: {options.InputPath}");

            return 1;
        }

        RuntimeAPIDocument? document;

        try
        {
            string json = File.ReadAllText(options.InputPath);

            document = JsonSerializer.Deserialize<RuntimeAPIDocument>(
                json,
                new JsonSerializerOptions
                {
                    PropertyNameCaseInsensitive = true
                });
        }
        catch (JsonException exception)
        {
            Console.Error.WriteLine(
                $"Failed to parse JSON: {exception.Message}");

            return 1;
        }
        catch (IOException exception)
        {
            Console.Error.WriteLine(
                $"Failed to read input file: {exception.Message}");

            return 1;
        }

        if (document is null)
        {
            Console.Error.WriteLine(
                "Input JSON did not contain a document.");

            return 1;
        }

        List<string> errors = RuntimeAPIValidator.Validate(document);

        if (errors.Count > 0)
        {
            Console.Error.WriteLine("Validation failed:");

            foreach (string error in errors)
                Console.Error.WriteLine($"- {error}");

            return 1;
        }

        string csharpSource;
        string cppHeaderSource;

        try
        {
            csharpSource = CSharpNativeAPIEmitter.Generate(document);
            cppHeaderSource = CppRuntimeAPIHeaderEmitter.Generate(document);
        }
        catch (InvalidOperationException exception)
        {
            Console.Error.WriteLine(
                $"Failed to generate source: {exception.Message}");

            return 1;
        }

        if (!TryWriteOutput(options.CSharpOutputPath, csharpSource) ||
            !TryWriteOutput(options.CppOutputPath, cppHeaderSource))
        {
            return 1;
        }

        Console.WriteLine($"Runtime API v{document.APIVersion}");

        foreach (RuntimeAPIDefinition api in document.APIs)
            Console.WriteLine($"[{api.Id}] {api.Module}.{api.Name}");

        Console.WriteLine("Generation succeeded.");
        return 0;
    }

    private static bool TryWriteOutput(
        string outputPath,
        string content)
    {
        try
        {
            string? outputDirectory = Path.GetDirectoryName(outputPath);

            if (!string.IsNullOrWhiteSpace(outputDirectory))
                Directory.CreateDirectory(outputDirectory);

            File.WriteAllText(outputPath, content);
            return true;
        }
        catch (IOException exception)
        {
            Console.Error.WriteLine(
                $"Failed to write '{outputPath}': {exception.Message}");

            return false;
        }
        catch (UnauthorizedAccessException exception)
        {
            Console.Error.WriteLine(
                $"Access denied for '{outputPath}': {exception.Message}");

            return false;
        }
    }

    private static bool TryGetOptions(
        string[] args,
        out GenerationOptions? options)
    {
        options = null;

        if (args.Length != 6 ||
            args[0] != "--input" ||
            args[2] != "--csharp-output" ||
            args[4] != "--cpp-output")
        {
            return false;
        }

        string inputPath = args[1];
        string csharpOutputPath = args[3];
        string cppOutputPath = args[5];

        if (string.IsNullOrWhiteSpace(inputPath) ||
            string.IsNullOrWhiteSpace(csharpOutputPath) ||
            string.IsNullOrWhiteSpace(cppOutputPath))
        {
            return false;
        }

        options = new GenerationOptions(
            inputPath,
            csharpOutputPath,
            cppOutputPath);

        return true;
    }

    private sealed record GenerationOptions(
        string InputPath,
        string CSharpOutputPath,
        string CppOutputPath);
}
