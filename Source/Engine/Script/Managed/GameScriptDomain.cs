using System.Reflection;
using System.Runtime.Loader;

namespace URay.ScriptHost;

internal sealed class GameScriptDomain
{
    private sealed class GameScriptLoadContext : AssemblyLoadContext
    {
        internal GameScriptLoadContext() : base(isCollectible: true)
        {
        }

        protected override Assembly? Load(AssemblyName assemblyName)
        {
            Assembly hostAssembly = typeof(ScriptComponent).Assembly;

            return AssemblyName.ReferenceMatchesDefinition(
                assemblyName,
                hostAssembly.GetName())
                ? hostAssembly
                : null;
        }
    }

    private GameScriptLoadContext? loadContext;

    internal Assembly? Assembly { get; private set; }

    internal bool Load(string assemblyPath)
    {
        if (loadContext is not null ||
            Assembly is not null ||
            string.IsNullOrWhiteSpace(assemblyPath) ||
            !File.Exists(assemblyPath))
        {
            return false;
        }

        try
        {
            byte[] assemblyBytes = File.ReadAllBytes(assemblyPath);

            if (assemblyBytes.Length == 0)
                return false;

            var context = new GameScriptLoadContext();

            using var assemblyStream = new MemoryStream(
                assemblyBytes,
                writable: false);

            Assembly assembly = context.LoadFromStream(assemblyStream);

            loadContext = context;
            Assembly = assembly;

            return true;
        }
        catch (BadImageFormatException)
        {
            return false;
        }
        catch (FileLoadException)
        {
            return false;
        }
        catch (IOException)
        {
            return false;
        }
    }

    internal bool Unload()
    {
        GameScriptLoadContext? context = loadContext;

        if (context is null || Assembly is null)
            return false;

        Assembly = null;
        loadContext = null;

        context.Unload();
        return true;
    }
}
