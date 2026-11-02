namespace URay;

public abstract class ScriptBehaviour
{
    public virtual void OnLoad() { }
    public virtual void OnUpdate(float deltaTime) { }
    public virtual void OnUnload() { }
}