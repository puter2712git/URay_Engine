namespace URay;

internal enum ScriptFieldType : int
{
    Unsupported = 0,
    Bool = 1,
    Int = 2,
    Float = 3,
    String = 4,
    Vector3 = 5
}

internal sealed record ScriptFieldInfo(
    string typeName,
    string name,
    ScriptFieldType type,
    object? defaultValue);
