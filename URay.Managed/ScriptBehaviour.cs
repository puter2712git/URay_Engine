namespace URay;

public abstract class ScriptBehaviour
{
    public Unit owner { get; internal set; } = null!;

    public virtual void OnLoad() { }
    public virtual void OnUpdate(float deltaTime) { }
    public virtual void OnUnload() { }
}