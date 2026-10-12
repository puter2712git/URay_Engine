using System.Collections.Generic;

namespace ScriptAPIGenerator;

public sealed class RuntimeAPIDocument
{
    public int APIVersion { get; init; }
    public List<RuntimeAPIDefinition> APIs { get; init; } = [];
}

public sealed class RuntimeAPIDefinition
{
    public int Id { get; init; }
    public string Module { get; init; } = "";
    public string Name { get; init; } = "";
    public string Result { get; init; } = "";
    public List<RuntimeAPIParameter> Parameters { get; init; } = [];
    public string NativeImplementation { get; init; } = "";
}

public sealed class RuntimeAPIParameter
{
    public string Name { get; init; } = "";
    public string Type { get; init; } = "";
    public string Direction { get; init; } = "";
}
