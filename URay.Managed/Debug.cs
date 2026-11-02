using System.Runtime.InteropServices;
using System.Text;

namespace URay;

public static unsafe class Debug
{
    private static delegate* unmanaged[Cdecl]<byte*, void> log;

    internal static void Initialize(delegate* unmanaged[Cdecl]<byte*, void> logFunction)
    {
        log = logFunction;
    }

    public static void Log(string message)
    {
        ArgumentNullException.ThrowIfNull(message);

        if (log == null)
        {
            throw new InvalidOperationException("Native logging is not initialized.");
        }

        if (message.Contains('\0'))
        {
            throw new ArgumentException("Message cannot contain a null character.", nameof(message));
        }

        byte[] utf8 = Encoding.UTF8.GetBytes(message + "\0");

        fixed (byte* messagePtr = utf8)
        {
            log(messagePtr);
        }
    }
}