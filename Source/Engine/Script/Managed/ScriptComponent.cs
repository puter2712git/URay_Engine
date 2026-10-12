namespace URay.ScriptHost;

public abstract class ScriptComponent
{
    private ulong managedInstanceId;

    protected Transform transform => new(managedInstanceId);

    protected virtual void OnInitialize() { }
    protected virtual void OnLoad() { }
    protected virtual void OnUpdate(float deltaTime) { }
    protected virtual void OnUnload() { }
    protected virtual void OnFinalize() { }

    internal void InvokeLifecycle(ScriptLifecycle lifecycle, float deltaTime)
    {
        switch (lifecycle)
        {
            case ScriptLifecycle.Initialize:
                OnInitialize();
                break;

            case ScriptLifecycle.Load:
                OnLoad();
                break;

            case ScriptLifecycle.Update:
                OnUpdate(deltaTime);
                break;

            case ScriptLifecycle.Unload:
                OnUnload();
                break;

            case ScriptLifecycle.Finalize:
                OnFinalize();
                break;
        }
    }

    internal void AttachManagedInstance(ulong instanceId)
    {
        if (instanceId == 0)
            throw new ArgumentOutOfRangeException(nameof(instanceId));

        managedInstanceId = instanceId;
    }

    internal void DetachManagedInstance()
    {
        managedInstanceId = 0;
    }
}