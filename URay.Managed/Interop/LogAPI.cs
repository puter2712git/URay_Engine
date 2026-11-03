using System.Runtime.InteropServices;

namespace URay;

[StructLayout(LayoutKind.Sequential)]
public unsafe struct LogAPI
{
    public delegate* unmanaged[Cdecl]<byte*, void> Log;
}
