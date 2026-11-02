using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace URay;

public static unsafe class EntryPoint
{
    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int Initialize(NativeAPI* api)
    {
        try
        {
            if (api == null || api->Log == null)
            {
                return -1;
            }

            Debug.Initialize(api->Log);

            Debug.Log("Hello from C#");
            return 0;
        }
        catch (Exception)
        {
            return -2;
        }
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static int CreateHelloScript()
    {
        try
        {
            return ScriptRuntime.CreateHelloScript();
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

}