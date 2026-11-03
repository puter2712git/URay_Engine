namespace URay;

public static unsafe class Input
{
    public static bool IsKeyHeld(KeyCode key)
    {
        var isKeyHeld = NativeBindings.api.inputAPI.IsKeyHeld;

        if (isKeyHeld == null)
            throw new InvalidOperationException("Input API is not initialized.");

        int result = isKeyHeld((int)key);

        if (result < 0)
            throw new ArgumentOutOfRangeException(nameof(key));

        return result == 1;
    }
}
