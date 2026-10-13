using System.ComponentModel.Design.Serialization;
using System.Text;

namespace URay.ScriptHost;

public static unsafe partial class URay
{
    private static NativeAPI nativeAPI;
    private static bool isInitialized;

    internal static void Initialize(NativeAPI nativeAPI)
    {
        URay.nativeAPI = nativeAPI;
        URay.isInitialized = true;
    }

    internal static bool RegisterScriptType(string className)
    {
        if (!isInitialized || string.IsNullOrEmpty(className))
            return false;

        if (nativeAPI.Bootstrap.RegisterScriptType == null)
            return false;

        byte[] classNameUTF8 = Encoding.UTF8.GetBytes(className);

        fixed (byte* classNamePtr = classNameUTF8)
        {
            int result = nativeAPI.Bootstrap.RegisterScriptType(
                nativeAPI.UserData,
                classNamePtr,
                classNameUTF8.Length);

            return result != 0;
        }
    }

    internal static bool RegisterScriptField<T>(
        string className,
        string fieldName,
        ScriptFieldType fieldType,
        T value)
        where T : unmanaged
    {
        if (!isInitialized ||
            string.IsNullOrEmpty(className) ||
            string.IsNullOrEmpty(fieldName))
        {
            return false;
        }

        if (nativeAPI.Bootstrap.RegisterScriptField == null)
            return false;

        byte[] classNameUTF8 = Encoding.UTF8.GetBytes(className);
        byte[] fieldNameUTF8 = Encoding.UTF8.GetBytes(fieldName);

        fixed (byte* classNamePtr = classNameUTF8)
        fixed (byte* fieldNamePtr = fieldNameUTF8)
        {
            T fieldValue = value;

            int result = nativeAPI.Bootstrap.RegisterScriptField(
                nativeAPI.UserData,
                classNamePtr,
                classNameUTF8.Length,
                fieldNamePtr,
                fieldNameUTF8.Length,
                (int)fieldType,
                &fieldValue,
                sizeof(T));

            return result != 0;
        }
    }

    internal static bool RegisterScriptField(
        string className,
        string fieldName,
        string? value)
    {
        if (!isInitialized ||
            string.IsNullOrEmpty(className) ||
            string.IsNullOrEmpty(fieldName))
        {
            return false;
        }

        if (nativeAPI.Bootstrap.RegisterScriptField == null)
            return false;

        byte[] classNameUTF8 = Encoding.UTF8.GetBytes(className);
        byte[] fieldNameUTF8 = Encoding.UTF8.GetBytes(fieldName);
        byte[] valueUTF8 = Encoding.UTF8.GetBytes(value ?? string.Empty);

        fixed (byte* classNamePtr = classNameUTF8)
        fixed (byte* fieldNamePtr = fieldNameUTF8)
        fixed (byte* valuePtr = valueUTF8)
        {
            int result = nativeAPI.Bootstrap.RegisterScriptField(
                nativeAPI.UserData,
                classNamePtr,
                classNameUTF8.Length,
                fieldNamePtr,
                fieldNameUTF8.Length,
                (int)ScriptFieldType.String,
                valuePtr,
                valueUTF8.Length);

            return result != 0;
        }
    }

}
