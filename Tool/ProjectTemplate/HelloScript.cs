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

        Vector3 direction = Vector3.Zero;

        if (Input.IsKeyHeld(KeyCode.W))
            direction += Vector3.Forward;

        if (Input.IsKeyHeld(KeyCode.S))
            direction -= Vector3.Forward;

        if (Input.IsKeyHeld(KeyCode.D))
            direction += Vector3.Right;

        if (Input.IsKeyHeld(KeyCode.A))
            direction -= Vector3.Right;

        Vector3 position = transform.localPosition;
        position += direction * (2.0f * deltaTime);
        transform.localPosition = position;
    }

    public override void OnUnload()
    {
    }
}
