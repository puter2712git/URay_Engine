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

            foreach (string typeName in ScriptRuntime.DiscoverScriptTypes())
            {
                Debug.Log($"Discovered script: {typeName}");
            }

            return 0;
        }
        catch (Exception)
        {
            return -2;
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
}