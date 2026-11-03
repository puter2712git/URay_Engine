using System.Runtime.InteropServices;

namespace URay;

[StructLayout(LayoutKind.Sequential)]
public unsafe struct ObjectAPI
{
    public delegate* unmanaged[Cdecl]<int, uint, int> IsValid;
}
