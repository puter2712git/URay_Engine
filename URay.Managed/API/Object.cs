namespace URay;

public abstract class Object
{
    internal ObjectId id { get; }

    internal Object(ObjectId id)
    {
        this.id = id;
    }

    public unsafe bool IsValid()
    {
        var isValid = NativeBindings.api.objectAPI.IsValid;

        if (isValid == null)
            throw new InvalidOperationException("Object API is not initialized.");

        int result = isValid(id.index, id.generation);

        if (result < 0)
            throw new InvalidOperationException("Failed to check object validity.");

        return result == 1;
    }
}
