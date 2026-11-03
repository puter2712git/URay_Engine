using System.Runtime.InteropServices;

namespace URay;

[StructLayout(LayoutKind.Sequential)]
public unsafe struct TransformAPI
{
    public delegate* unmanaged[Cdecl]<int, uint, Vector3*, int> GetLocalPosition;
    public delegate* unmanaged[Cdecl]<int, uint, Vector3*, int> SetLocalPosition;
}
