using System.Runtime.InteropServices;

namespace URay;

[StructLayout(LayoutKind.Sequential)]
public unsafe struct UnitAPI
{
    public delegate* unmanaged[Cdecl]<int, uint, int*, uint*, int> GetTransform;
}
