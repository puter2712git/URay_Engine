using System.Runtime.InteropServices;

namespace URay;

[StructLayout(LayoutKind.Sequential)]
public unsafe struct NativeAPI
{
    public LogAPI logAPI;
    public ObjectAPI objectAPI;
    public UnitAPI unitAPI;
    public TransformAPI transformAPI;
}

