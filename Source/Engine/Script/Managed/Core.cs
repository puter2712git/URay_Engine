namespace URay.ScriptHost;

public sealed class Core
{
    public static void Log(string message)
    {
        if (!URay.TryLog(message))
        {
            throw new InvalidOperationException(
                "Failed to write an engine log message.");
        }
    }
}
