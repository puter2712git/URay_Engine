namespace URay;

internal static class ScriptRegistry
{
    private static readonly Dictionary<int, ScriptBehaviour> instances = new();
    private static int nextHandle = 1;

    public static int Add(ScriptBehaviour instance)
    {
        int handle = nextHandle;
        nextHandle = checked(nextHandle + 1);

        instances.Add(handle, instance);
        return handle;
    }

    public static bool TryGet(int handle, out ScriptBehaviour? instance)
    {
        return instances.TryGetValue(handle, out instance);
    }

    public static bool Remove(int handle, out ScriptBehaviour? instance)
    {
        return instances.Remove(handle, out instance);
    }
}