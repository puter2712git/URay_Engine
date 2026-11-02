namespace URay;

internal static class ScriptRuntime
{
    public static int CreateHelloScript()
    {
        var instance = new HelloScript();
        int handle = ScriptRegistry.Add(instance);

        try
        {
            instance.OnLoad();
            return handle;
        }
        catch
        {
            ScriptRegistry.Remove(handle, out _);
            throw;
        }
    }

    public static bool Update(int handle, float deltaTime)
    {
        if (!ScriptRegistry.TryGet(handle, out ScriptBehaviour? instance) || instance == null)
        {
            return false;
        }

        instance.OnUpdate(deltaTime);
        return true;
    }

    public static bool Unload(int handle)
    {
        if (!ScriptRegistry.Remove(handle, out ScriptBehaviour? instance) || instance == null)
        {
            return false;
        }

        instance.OnUnload();
        return true;
    }
}
