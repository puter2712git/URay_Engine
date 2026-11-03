using System.Text;

namespace URay;

public static unsafe class Debug
{
    public static void Log(string message)
    {
        ArgumentNullException.ThrowIfNull(message);

        var log = NativeBindings.api.logAPI.Log;

        if (log == null)
            throw new InvalidOperationException("Native logging is not initialized.");

        if (message.Contains('\0'))
            throw new ArgumentException("Message cannot contain a null character.", nameof(message));

        byte[] utf8 = Encoding.UTF8.GetBytes(message + "\0");

        fixed (byte* messagePtr = utf8)
        {
            log(messagePtr);
        }
    }
}