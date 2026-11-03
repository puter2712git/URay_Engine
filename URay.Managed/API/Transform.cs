namespace URay;

public sealed class Transform : Object
{
    internal Transform(ObjectId id) : base(id)
    {
    }

    public unsafe Vector3 localPosition
    {
        get
        {
            var getPosition = NativeBindings.api.transformAPI.GetLocalPosition;

            if (getPosition == null)
                throw new InvalidOperationException("Transform API is not initialized.");

            Vector3 position = default;

            int result = getPosition(
                id.index,
                id.generation,
                &position);

            if (result == 0)
                throw new InvalidOperationException("Transform is not valid.");

            if (result != 1)
                throw new InvalidOperationException("Failed to get local position.");

            return position;
        }
        set
        {
            var setPosition = NativeBindings.api.transformAPI.SetLocalPosition;

            if (setPosition == null)
                throw new InvalidOperationException("Transform API is not initialized.");

            int result = setPosition(
                id.index,
                id.generation,
                &value);

            if (result == 0)
                throw new InvalidOperationException("Transform is not valid.");

            if (result != 1)
                throw new InvalidOperationException("Failed to set local position.");
        }
    }
}
