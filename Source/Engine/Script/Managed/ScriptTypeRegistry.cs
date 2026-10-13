using System.Reflection;

namespace URay.ScriptHost;

internal static class ScriptTypeRegistry
{
    private const BindingFlags FieldFlags =
        BindingFlags.Instance |
        BindingFlags.Public |
        BindingFlags.NonPublic |
        BindingFlags.DeclaredOnly;

    internal static bool RegisterAll(Assembly assembly)
    {
        ArgumentNullException.ThrowIfNull(assembly);

        try
        {
            IEnumerable<Type> scriptTypes = assembly
                .GetTypes()
                .Where(IsConcreteScriptType)
                .OrderBy(type => type.FullName, StringComparer.Ordinal);

            foreach (Type scriptType in scriptTypes)
            {
                if (!RegisterType(scriptType))
                    return false;
            }

            return true;
        }
        catch (Exception exception)
        {
            Core.Log($"Failed to register script types: {exception}");
            return false;
        }
    }

    private static bool IsConcreteScriptType(Type type)
    {
        return typeof(ScriptComponent).IsAssignableFrom(type) &&
            type != typeof(ScriptComponent) &&
            !type.IsAbstract &&
            !type.ContainsGenericParameters &&
            type.FullName is not null;
    }

    private static bool RegisterType(Type scriptType)
    {
        string className = scriptType.FullName!;

        object? defaultInstance = Activator.CreateInstance(scriptType);

        if (defaultInstance is null)
        {
            Core.Log($"Failed to create default instance: {className}");
            return false;
        }

        if (!URay.RegisterScriptType(className))
        {
            Core.Log($"Failed to register script type: {className}");
            return false;
        }

        IEnumerable<FieldInfo> fields = scriptType
            .GetFields(FieldFlags)
            .Where(IsSerializableField)
            .OrderBy(field => field.Name, StringComparer.Ordinal);

        foreach (FieldInfo field in fields)
        {
            if (!RegisterField(className, defaultInstance, field))
                return false;
        }

        return true;
    }

    private static bool IsSerializableField(FieldInfo field)
    {
        return !field.IsStatic &&
            !field.IsLiteral &&
            !field.IsInitOnly &&
            field.GetCustomAttribute<SerializeFieldAttribute>() is not null;
    }

    private static bool RegisterField(
        string className,
        object defaultInstance,
        FieldInfo field)
    {
        object? value = field.GetValue(defaultInstance);

        if (field.FieldType == typeof(bool))
        {
            bool fieldValue = value is bool boolValue && boolValue;

            return URay.RegisterScriptField(
                className,
                field.Name,
                ScriptFieldType.Bool,
                fieldValue ? (byte)1 : (byte)0);
        }

        if (field.FieldType == typeof(int))
        {
            int fieldValue = value is int intValue ? intValue : 0;

            return URay.RegisterScriptField(
                className,
                field.Name,
                ScriptFieldType.Int,
                fieldValue);
        }

        if (field.FieldType == typeof(float))
        {
            float fieldValue = value is float floatValue ? floatValue : 0;

            return URay.RegisterScriptField(
                className,
                field.Name,
                ScriptFieldType.Float,
                fieldValue);
        }

        if (field.FieldType == typeof(string))
        {
            return URay.RegisterScriptField(
                className,
                field.Name,
                value as string ?? string.Empty);
        }

        Core.Log(
            $"Unsupported serialized script field: " +
            $"{className}.{field.Name} ({field.FieldType.FullName})");

        return false;
    }
}