namespace URay.ScriptHost;

public sealed class Transform
{
    private readonly ulong managedInstanceId;

    internal Transform(ulong managedInstanceId)
    {
        this.managedInstanceId = managedInstanceId;
    }

    public Vector3 Position
    {
        get
        {
            if (URay.TryGetTransformPosition(
                managedInstanceId,
                out Vector3 position))
            {
                return position;
            }

            throw new InvalidOperationException(
                "The native Transform is no longer available.");
        }

        set
        {
            if (URay.TrySetTransformPosition(
                managedInstanceId,
                value))
            {
                return;
            }

            throw new InvalidOperationException(
                "The native Transform is no longer available.");
        }
    }
}