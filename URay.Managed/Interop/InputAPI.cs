using System.Runtime.InteropServices;

namespace URay;

[StructLayout(LayoutKind.Sequential)]
public unsafe struct InputAPI
{
    public delegate* unmanaged[Cdecl]<int, int> IsKeyHeld;
}
