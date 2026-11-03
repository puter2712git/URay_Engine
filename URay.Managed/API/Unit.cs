namespace URay;

public sealed class Unit : Object
{
    internal Unit(ObjectId id) : base(id)
    {
    }

    public unsafe Transform? transform
    {
        get
        {
            var getTransform = NativeBindings.api.unitAPI.GetTransform;

            if (getTransform == null)
                throw new InvalidOperationException("Unit API is not initialized.");

            int transformIndex = -1;
            uint transformGeneration = 0;

            int result = getTransform(
                id.index,
                id.generation,
                &transformIndex,
                &transformGeneration);

            if (result < 0)
                throw new InvalidOperationException("Failed to get transform.");

            if (result == 0)
                return null;

            return new Transform(new ObjectId(transformIndex, transformGeneration));
        }
    }
}
