namespace URay;

public sealed class HelloScript : ScriptBehaviour
{
    public override void OnLoad()
    {
    }

    public override void OnUpdate(float deltaTime)
    {
        Transform? transform = owner.transform;
        if (transform == null)
            return;

        Vector3 position = transform.localPosition;
        position += Vector3.Right * (2.0f * deltaTime);
        transform.localPosition = position;
    }

    public override void OnUnload()
    {
    }
}
