namespace URay.ScriptHost;

public static class Input
{
    public static bool GetKey(KeyCode key)
    {
        return URay.TryGetKey((ushort)key);
    }

    public static bool GetKeyDown(KeyCode key)
    {
        return URay.TryGetKeyDown((ushort)key);
    }

    public static bool GetKeyUp(KeyCode key)
    {
        return URay.TryGetKeyUp((ushort)key);
    }
}