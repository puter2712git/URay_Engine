namespace URay;

internal static class NativeBindings
{
    internal static NativeAPI api { get; private set; }

    internal static void Initialize(NativeAPI nativeAPI)
    {
        api = nativeAPI;
    }
}
