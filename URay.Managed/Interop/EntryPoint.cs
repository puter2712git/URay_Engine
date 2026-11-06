using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace URay;

public static unsafe class EntryPoint
{
    [StructLayout(LayoutKind.Sequential)]
    public struct ScriptFieldTransfer
    {
        public byte* declaringTypeName;
        public byte* name;
        public int type;
        public byte* data;
        public int size;
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int Initialize(NativeAPI* api)
    {
        try
        {
            if (api == null ||
                api->logAPI.Log == null ||
                api->inputAPI.IsKeyHeld == null ||
                api->objectAPI.IsValid == null ||
                api->unitAPI.GetTransform == null ||
                api->transformAPI.GetLocalPosition == null ||
                api->transformAPI.SetLocalPosition == null)
            {
                return -1;
            }

            if (sizeof(Vector3) != 12 ||
                Marshal.OffsetOf<Vector3>(nameof(Vector3.x)).ToInt32() != 0 ||
                Marshal.OffsetOf<Vector3>(nameof(Vector3.y)).ToInt32() != 4 ||
                Marshal.OffsetOf<Vector3>(nameof(Vector3.z)).ToInt32() != 8)
            {
                return -1;
            }

            NativeBindings.Initialize(*api);

            return 0;
        }
        catch (Exception)
        {
            return -2;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int LoadProjectAssembly(byte* assemblyPath)
    {
        try
        {
            if (assemblyPath == null)
                return -1;

            string? path = Marshal.PtrToStringUTF8((nint)assemblyPath);

            if (string.IsNullOrEmpty(path))
                return -1;

            if (!File.Exists(path))
                return 1;

            return ScriptRuntime.LoadProjectAssembly(path) ? 0 : -1;
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine($"[ScriptRuntime] Failed to load project assembly: {exception}");
            return -1;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int CreateScript(
        byte* typeName,
        int ownerIndex,
        uint ownerGeneration,
        ScriptFieldTransfer* fields,
        int fieldCount)
    {
        try
        {
            if (typeName == null ||
                fieldCount < 0 ||
                (fieldCount > 0 && fields == null))
            {
                return 0;
            }

            string? scriptName = Marshal.PtrToStringUTF8((nint)typeName);
            if (string.IsNullOrEmpty(scriptName))
                return 0;

            List<ScriptFieldOverride> overrides = new(fieldCount);

            for (int i = 0; i < fieldCount; ++i)
            {
                ScriptFieldTransfer field = fields[i];

                string? declaringTypeName = Marshal.PtrToStringUTF8((nint)field.declaringTypeName);
                string? fieldName = Marshal.PtrToStringUTF8((nint)field.name);

                if (string.IsNullOrEmpty(declaringTypeName) ||
                    string.IsNullOrEmpty(fieldName))
                {
                    return 0;
                }

                ScriptFieldType fieldType = (ScriptFieldType)field.type;
                object? value;

                if (fieldType == ScriptFieldType.String && field.size == -1)
                {
                    if (field.data != null)
                        return 0;

                    value = null;
                }
                else
                {
                    if (field.size < 0 || (field.size > 0 && field.data == null))
                        return 0;

                    ReadOnlySpan<byte> data = new ReadOnlySpan<byte>(field.data, field.size);

                    switch (fieldType)
                    {
                        case ScriptFieldType.Bool:
                        {
                            if (data.Length != sizeof(int))
                                return 0;

                            int raw = MemoryMarshal.Read<int>(data);

                            if (raw != 0 && raw != 1)
                                return 0;

                            value = raw == 1;
                            break;
                        }

                        case ScriptFieldType.Int:
                        {
                            if (data.Length != sizeof(int))
                                return 0;

                            value = MemoryMarshal.Read<int>(data);
                            break;
                        }

                        case ScriptFieldType.Float:
                        {
                            if (data.Length != sizeof(float))
                                return 0;

                            value = MemoryMarshal.Read<float>(data);
                            break;
                        }

                        case ScriptFieldType.Vector3:
                        {
                            if (data.Length != sizeof(float) * 3)
                                return 0;

                            value = new Vector3(
                                MemoryMarshal.Read<float>(data),
                                MemoryMarshal.Read<float>(data.Slice(sizeof(float))),
                                MemoryMarshal.Read<float>(data.Slice(sizeof(float) * 2)));
                            break;
                        }

                        case ScriptFieldType.String:
                        {
                            value = Encoding.UTF8.GetString(data);
                            break;
                        }

                        default:
                            return 0;
                    }
                }

                overrides.Add(new ScriptFieldOverride(
                    declaringTypeName,
                    fieldName,
                    fieldType,
                    value));
            }

            return ScriptRuntime.CreateScript(
                scriptName,
                ownerIndex,
                ownerGeneration,
                overrides);
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine($"[ScriptRuntime] Failed to create script: {exception}");
            return 0;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int UpdateScript(int handle, float deltaTime)
    {
        try
        {
            return ScriptRuntime.Update(handle, deltaTime) ? 0 : -1;
        }
        catch (Exception)
        {
            return -2;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int UnloadScript(int handle)
    {
        try
        {
            return ScriptRuntime.Unload(handle) ? 0 : -1;
        }
        catch (Exception)
        {
            return -2;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int EnumerateScriptTypes(
        delegate* unmanaged[Cdecl]<void*, byte*, int> callback,
        void* context)
    {
        if (callback == null || context == null)
            return -1;

        try
        {
            foreach (string typeName in ScriptRuntime.DiscoverScriptTypes())
            {
                byte[] utf8 = Encoding.UTF8.GetBytes(typeName + "\0");

                fixed (byte* namePtr = utf8)
                {
                    if (callback(context, namePtr) != 0)
                        return -2;
                }
            }

            return 0;
        }
        catch (Exception)
        {
            return -3;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int EnumerateScriptFields(
        byte* typeName,
        delegate* unmanaged[Cdecl]<void*, byte*, byte*, int, byte*, int, int> callback,
        void* context)
    {
        if (typeName == null || callback == null || context == null)
            return -1;

        try
        {
            string? name = Marshal.PtrToStringUTF8((nint)typeName);

            if (string.IsNullOrEmpty(name) ||
                !ScriptRuntime.DiscoverScriptTypes().Contains(name, StringComparer.Ordinal))
            {
                return -1;
            }

            foreach (ScriptFieldInfo field in ScriptRuntime.DiscoverScriptFields(name))
            {
                byte[] declaringTypeUTF8 = Encoding.UTF8.GetBytes(field.typeName + "\0");
                byte[] fieldNameUTF8 = Encoding.UTF8.GetBytes(field.name + "\0");

                fixed (byte* declaringTypePtr = declaringTypeUTF8)
                fixed (byte* fieldNamePtr = fieldNameUTF8)
                {
                    int result = SendScriptField(
                        field,
                        callback,
                        context,
                        declaringTypePtr,
                        fieldNamePtr);

                    if (result != 0)
                        return -2;
                }
            }

            return 0;
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine($"[ScriptRuntime] Failed to enumerate script fields: {exception}");
            return -3;
        }
    }

    private static int SendScriptField(
        ScriptFieldInfo field,
        delegate* unmanaged[Cdecl]<void*, byte*, byte*, int, byte*, int, int> callback,
        void* context,
        byte* declaringTypeName,
        byte* fieldName)
    {
        switch (field.type)
        {
            case ScriptFieldType.Bool:
            {
                if (field.defaultValue is not bool value)
                    return -1;

                int data = value ? 1 : 0;

                return callback(
                    context, declaringTypeName, fieldName,
                    (int)field.type, (byte*)&data, sizeof(int));
            }

            case ScriptFieldType.Int:
            {
                if (field.defaultValue is not int value)
                    return -1;

                return callback(
                    context, declaringTypeName, fieldName,
                    (int)field.type, (byte*)&value, sizeof(int));
            }

            case ScriptFieldType.Float:
            {
                if (field.defaultValue is not float value)
                    return -1;

                return callback(
                    context, declaringTypeName, fieldName,
                    (int)field.type, (byte*)&value, sizeof(float));
            }

            case ScriptFieldType.Vector3:
            {
                if (field.defaultValue is not Vector3 value)
                    return -1;

                float* data = stackalloc float[3];
                data[0] = value.x;
                data[1] = value.y;
                data[2] = value.z;

                return callback(
                    context, declaringTypeName, fieldName,
                    (int)field.type, (byte*)data, sizeof(float) * 3);
            }

            case ScriptFieldType.String:
            {
                if (field.defaultValue == null)
                {
                    return callback(
                        context, declaringTypeName, fieldName,
                        (int)field.type, null, -1);
                }

                if (field.defaultValue is not string value)
                    return -1;

                byte[] utf8 = Encoding.UTF8.GetBytes(value);

                fixed (byte* data = utf8)
                {
                    return callback(
                        context, declaringTypeName, fieldName,
                        (int)field.type, data, utf8.Length);
                }
            }

            default:
                return -1;
        }
    }
}