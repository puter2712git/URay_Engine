
using System.Runtime.InteropServices;

namespace URay;

[StructLayout(LayoutKind.Sequential)]
public unsafe struct NativeAPI
{
    public delegate* unmanaged[Cdecl]<byte*, void> Log;
}

