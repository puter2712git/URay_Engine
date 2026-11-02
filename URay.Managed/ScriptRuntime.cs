namespace URay;

internal static class ScriptRuntime
{
    public static int CreateScript(string typeName)
    {
        if (!DiscoverScriptTypes().Contains(typeName, StringComparer.Ordinal))
            return 0;

        Type? type = typeof(ScriptBehaviour).Assembly.GetType(
            typeName,
            throwOnError: false,
            ignoreCase: false);

        if (type == null)
            return 0;

        if (Activator.CreateInstance(type) is not ScriptBehaviour instance)
            return 0;

        int handle = ScriptRegistry.Add(instance);

        try
        {
            instance.OnLoad();
            return handle;
        }
        catch
        {
            ScriptRegistry.Remove(handle, out _);
            throw;
        }
    }

    public static bool Update(int handle, float deltaTime)
    {
        if (!ScriptRegistry.TryGet(handle, out ScriptBehaviour? instance) || instance == null)
        {
            return false;
        }

        instance.OnUpdate(deltaTime);
        return true;
    }

    public static bool Unload(int handle)
    {
        if (!ScriptRegistry.Remove(handle, out ScriptBehaviour? instance) || instance == null)
        {
            return false;
        }

        instance.OnUnload();
        return true;
    }

    public static string[] DiscoverScriptTypes()
    {
        return typeof(ScriptBehaviour).Assembly
            .GetTypes()
            .Where(type =>
                type.IsClass &&
                type.IsPublic &&
                !type.IsAbstract &&
                !type.ContainsGenericParameters &&
                typeof(ScriptBehaviour).IsAssignableFrom(type) &&
                type.GetConstructor(Type.EmptyTypes) != null)
            .Select(type => type.FullName!)
            .OrderBy(name => name, StringComparer.Ordinal)
            .ToArray();
    }
}
