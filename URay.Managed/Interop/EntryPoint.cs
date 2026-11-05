using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace URay;

public static unsafe class EntryPoint
{
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
    public static int CreateScript(byte* typeName, int ownerIndex, uint ownerGeneration)
    {
        try
        {
            if (typeName == null)
                return 0;

            string? name = Marshal.PtrToStringUTF8((nint)typeName);

            if (string.IsNullOrEmpty(name))
                return 0;

            return ScriptRuntime.CreateScript(name, ownerIndex, ownerGeneration);
        }
        catch (Exception)
        {
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
        delegate* unmanaged[Cdecl]<void*, byte*, byte*, int, int> callback,
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
                    int result = callback(
                        context,
                        declaringTypePtr,
                        fieldNamePtr,
                        (int)field.type);

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
}