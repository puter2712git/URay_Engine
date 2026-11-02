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
}