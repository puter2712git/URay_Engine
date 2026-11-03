namespace URay;

public sealed class HelloScript : ScriptBehaviour
{
    public override void OnLoad()
    {
        Debug.Log($"Owner is valid: {owner.IsValid()}");
    }

    public override void OnUpdate(float deltaTime)
    {
        Debug.Log($"Update deltaTime: {deltaTime}");
    }

    public override void OnUnload()
    {
        Debug.Log("Unloaded");
    }
}
