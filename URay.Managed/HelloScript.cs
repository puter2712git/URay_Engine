namespace URay;

public sealed class HelloScript : ScriptBehaviour
{
    public override void OnLoad()
    {
        Transform? transform = owner.transform;

        if (transform == null)
        {
            Debug.Log("Transform was not found.");
            return;
        }

        Debug.Log($"Transform is valid: {transform.IsValid()}");
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
