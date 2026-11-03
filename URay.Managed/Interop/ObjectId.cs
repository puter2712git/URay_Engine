namespace URay;

internal readonly struct ObjectId
{
    public int index { get; }
    public uint generation { get; }

    public ObjectId(int index, uint generation)
    {
        this.index = index;
        this.generation = generation;
    }
}
