using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace URay.ScriptHost;

internal enum ScriptLifecycle
{
    Initialize = 0,
    Load = 1,
    Update = 2,
    Unload = 3,
    Finalize = 4
}

public static unsafe class ScriptEntry
{
    private sealed class ManagedScriptInstance
    {
        public required ScriptComponent Component;
        public required Dictionary<string, FieldInfo> Fields;
    }

    private static readonly GameScriptDomain gameScriptDomain = new();

    private static readonly Dictionary<ulong, ManagedScriptInstance> instances = [];
    private static ulong nextInstanceId = 1;

    private const BindingFlags FieldFlags =
        BindingFlags.Instance |
        BindingFlags.Public |
        BindingFlags.NonPublic |
        BindingFlags.DeclaredOnly;

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static int Initialize(NativeAPI* nativeAPI, int nativeAPISize)
    {
        if (nativeAPI is null || nativeAPISize != sizeof(NativeAPI))
            return -1;

        URay.Initialize(*nativeAPI);

        return 0;
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static int LoadGameScripts(
        byte* assemblyPathData,
        int assemblyPathLength)
    {
        try
        {
            if (assemblyPathData is null || assemblyPathLength <= 0)
                return -1;

            string assemblyPath = Encoding.UTF8.GetString(
                new ReadOnlySpan<byte>(
                    assemblyPathData,
                    assemblyPathLength));

            if (!gameScriptDomain.Load(assemblyPath))
                return -2;

            Assembly? assembly = gameScriptDomain.Assembly;

            return assembly is not null &&
                ScriptTypeRegistry.RegisterAll(assembly)
                ? 0
                : -3;
        }
        catch (Exception exception)
        {
            URay.Log($"Failed to load game scripts: {exception}");
            return -4;
        }
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static int UnloadGameScripts()
    {
        if (instances.Count != 0)
            return -1;

        return gameScriptDomain.Unload()
            ? 0
            : -2;
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static ulong CreateScriptInstance(byte* classNameData, int classNameLength)
    {
        try
        {
            if (classNameData is null || classNameLength <= 0)
                return 0;

            string className = Encoding.UTF8.GetString(
                new ReadOnlySpan<byte>(
                    classNameData,
                    classNameLength));

            Type? type = gameScriptDomain.Assembly?.GetType(className);

            if (type is null ||
                type.IsAbstract ||
                !typeof(ScriptComponent).IsAssignableFrom(type))
            {
                URay.Log($"Script type was not found: {className}");
                return 0;
            }

            if (Activator.CreateInstance(type) is not ScriptComponent component)
            {
                return 0;
            }

            ulong instanceId = nextInstanceId++;
            if (instanceId == 0)
                instanceId = nextInstanceId++;

            component.AttachManagedInstance(instanceId);

            instances.Add(instanceId,
                new ManagedScriptInstance
                {
                    Component = component,
                    Fields = BuildSerializableFieldMap(type)
                });

            return instanceId;
        }
        catch (Exception exception)
        {
            URay.Log($"Failed to create script instance: {exception}");
            return 0;
        }
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static int SetScriptFieldValue(
        ulong instanceId,
        byte* fieldNameData,
        int fieldNameLength,
        int fieldType,
        void* valueData,
        int valueSize)
    {
        try
        {
            if (fieldNameData is null ||
                fieldNameLength <= 0 ||
                valueSize < 0)
            {
                return 0;
            }

            if (!instances.TryGetValue(
                    instanceId,
                    out ManagedScriptInstance? instance))
            {
                return 0;
            }

            string fieldName = Encoding.UTF8.GetString(
                new ReadOnlySpan<byte>(
                    fieldNameData,
                    fieldNameLength));

            if (!instance.Fields.TryGetValue(
                    fieldName,
                    out FieldInfo? field))
            {
                return 0;
            }

            if (!TryReadFieldValue(
                    fieldType,
                    valueData,
                    valueSize,
                    out object? value))
            {
                return 0;
            }

            if (value is null ||
                field.FieldType != value.GetType())
            {
                return 0;
            }

            field.SetValue(instance.Component, value);
            return 1;
        }
        catch (Exception exception)
        {
            URay.Log(
                $"Failed to set script field: {exception}");

            return 0;
        }
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static int InvokeScriptLifecycle(
        ulong instanceId,
        int lifecycleValue,
        float deltaTime)
    {
        try
        {
            if (!instances.TryGetValue(
                    instanceId,
                    out ManagedScriptInstance? instance))
            {
                return 0;
            }

            ScriptLifecycle lifecycle =
                (ScriptLifecycle)lifecycleValue;

            switch (lifecycle)
            {
                case ScriptLifecycle.Initialize:
                case ScriptLifecycle.Load:
                case ScriptLifecycle.Update:
                case ScriptLifecycle.Unload:
                case ScriptLifecycle.Finalize:
                    instance.Component.InvokeLifecycle(
                        lifecycle,
                        deltaTime);
                    return 1;

                default:
                    return 0;
            }
        }
        catch (Exception exception)
        {
            URay.Log($"Script lifecycle invocation failed: {exception}");
            return 0;
        }
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static void DestroyScriptInstance(ulong instanceId)
    {
        if (instances.Remove(instanceId, out ManagedScriptInstance? instance))
        {
            instance.Component.DetachManagedInstance();
        }
    }

    private static Dictionary<string, FieldInfo> BuildSerializableFieldMap(Type type)
    {
        return type
            .GetFields(FieldFlags)
            .Where(field =>
                !field.IsStatic &&
                !field.IsLiteral &&
                !field.IsInitOnly &&
                field.GetCustomAttribute<SerializeFieldAttribute>()
                    is not null)
            .ToDictionary(
                field => field.Name,
                StringComparer.Ordinal);
    }

    private static bool TryReadFieldValue(
    int fieldType,
    void* valueData,
    int valueSize,
    out object? value)
    {
        value = null;

        switch ((ScriptFieldType)fieldType)
        {
            case ScriptFieldType.Bool:
                if (valueData is null || valueSize != 1)
                    return false;

                value = *(byte*)valueData != 0;
                return true;

            case ScriptFieldType.Int:
                if (valueData is null || valueSize != sizeof(int))
                    return false;

                value = *(int*)valueData;
                return true;

            case ScriptFieldType.Float:
                if (valueData is null || valueSize != sizeof(float))
                    return false;

                value = *(float*)valueData;
                return true;

            case ScriptFieldType.String:
                if (valueSize == 0)
                {
                    value = string.Empty;
                    return true;
                }

                if (valueData is null)
                    return false;

                value = Encoding.UTF8.GetString(
                    new ReadOnlySpan<byte>(
                        valueData,
                        valueSize));

                return true;

            default:
                return false;
        }
    }
}
