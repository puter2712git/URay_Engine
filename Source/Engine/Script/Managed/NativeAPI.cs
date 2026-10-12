using System.Runtime.InteropServices;

namespace URay.ScriptHost;

[StructLayout(LayoutKind.Sequential)]
public unsafe struct BootstrapAPI
{
    public delegate* unmanaged[Cdecl]<nint, byte*, int, void> LogUTF8;

    public delegate* unmanaged[Cdecl]<
        nint, byte*, int, int> RegisterScriptType;

    public delegate* unmanaged[Cdecl]<
        nint,
        byte*, int,
        byte*, int,
        int,
        void*, int,
        int> RegisterScriptField;
}

[StructLayout(LayoutKind.Sequential)]
public unsafe struct NativeAPI
{
    public nint UserData;
    public BootstrapAPI Bootstrap;
    public RuntimeAPI Runtime;
}