namespace ScriptAPIGenerator;

public static class RuntimeAPIValidator
{
    private static readonly HashSet<string> SupportedResultTypes =
        new(StringComparer.Ordinal)
        {
            "bool"
        };

    private static readonly HashSet<string> SupportedParameterTypes =
        new(StringComparer.Ordinal)
        {
            "u16",
            "u64",
            "Vector3",
            "string"
        };

    private static readonly HashSet<string> SupportedDirections =
        new(StringComparer.Ordinal)
        {
            "in",
            "out"
        };

    public static List<string> Validate(RuntimeAPIDocument document)
    {
        List<string> errors = [];

        if (document.APIVersion < 1)
            errors.Add("APIVersion must be greater than or equal to 1.");

        if (document.APIs.Count == 0)
            errors.Add("APIs must contain at least one API definition.");

        HashSet<int> ids = [];
        HashSet<string> qualifiedNames = new(StringComparer.Ordinal);

        int previousId = 0;

        foreach (RuntimeAPIDefinition api in document.APIs)
        {
            string apiName = GetDisplayName(api);

            ValidateAPIIdentity(
                api,
                apiName,
                ids,
                qualifiedNames,
                errors);

            if (api.Id <= previousId)
            {
                errors.Add(
                    $"API '{apiName}' id {api.Id} must be greater than " +
                    $"the previous id {previousId}.");
            }

            previousId = api.Id;

            ValidateResult(api, apiName, errors);
            ValidateParameters(api, apiName, errors);
        }

        return errors;
    }

    private static void ValidateAPIIdentity(
        RuntimeAPIDefinition api,
        string apiName,
        HashSet<int> ids,
        HashSet<string> qualifiedNames,
        List<string> errors)
    {
        if (api.Id <= 0)
            errors.Add($"API '{apiName}' id must be greater than zero.");

        if (!ids.Add(api.Id))
            errors.Add($"API id {api.Id} is duplicated.");

        if (string.IsNullOrWhiteSpace(api.Module))
            errors.Add($"API id {api.Id} has an empty module.");

        if (string.IsNullOrWhiteSpace(api.Name))
            errors.Add($"API id {api.Id} has an empty name.");

        if (!string.IsNullOrWhiteSpace(api.Module) &&
            !string.IsNullOrWhiteSpace(api.Name) &&
            !qualifiedNames.Add(apiName))
        {
            errors.Add($"API name '{apiName}' is duplicated.");
        }

        if (string.IsNullOrWhiteSpace(api.NativeImplementation))
        {
            errors.Add($"API '{apiName}' has an empty native implementation.");
        }
    }

    private static void ValidateResult(
        RuntimeAPIDefinition api,
        string apiName,
        List<string> errors)
    {
        if (!SupportedResultTypes.Contains(api.Result))
        {
            errors.Add(
                $"API '{apiName}' has unsupported result type " +
                $"'{api.Result}'.");
        }
    }

    private static void ValidateParameters(
        RuntimeAPIDefinition api,
        string apiName,
        List<string> errors)
    {
        HashSet<string> parameterNames = new(StringComparer.Ordinal);
        int outParameterCount = 0;
        int stringParameterCount = 0;

        int instanceIdIndex = -1;

        for (int index = 0; index < api.Parameters.Count; ++index)
        {
            RuntimeAPIParameter parameter = api.Parameters[index];

            if (string.IsNullOrWhiteSpace(parameter.Name))
            {
                errors.Add($"API '{apiName}' contains a parameter with an empty name.");
            }
            else if (!parameterNames.Add(parameter.Name))
            {
                errors.Add(
                    $"API '{apiName}' has duplicate parameter name " +
                    $"'{parameter.Name}'.");
            }

            if (parameter.Name == "instanceId")
                instanceIdIndex = index;

            if (!SupportedParameterTypes.Contains(parameter.Type))
            {
                errors.Add(
                    $"API '{apiName}' parameter '{parameter.Name}' has " +
                    $"unsupported type '{parameter.Type}'.");
            }

            if (!SupportedDirections.Contains(parameter.Direction))
            {
                errors.Add(
                    $"API '{apiName}' parameter '{parameter.Name}' has " +
                    $"unsupported direction '{parameter.Direction}'.");

                continue;
            }

            if (parameter.Direction == "out" &&
                parameter.Type != "Vector3")
            {
                errors.Add(
                    $"API '{apiName}' parameter '{parameter.Name}' uses " +
                    $"'out', but only 'Vector3' supports 'out'.");
            }

            if (parameter.Direction == "out" && ++outParameterCount > 1)
            {
                errors.Add(
                    $"API '{apiName}' has multiple out parameters. " +
                    "Only one out Vector3 is currently supported.");
            }

            if (parameter.Type == "string")
            {
                ++stringParameterCount;

                if (parameter.Direction != "in")
                {
                    errors.Add(
                        $"API '{apiName}' parameter '{parameter.Name}' uses " +
                        $"'string', but only 'in string' is currently supported.");
                }
            }
        }

        if (stringParameterCount > 1)
        {
            errors.Add(
                $"API '{apiName}' has multiple string parameters. " +
                "Only one in string is currently supported.");
        }

        if (stringParameterCount > 0 && outParameterCount > 0)
        {
            errors.Add(
                $"API '{apiName}' mixes string and out parameters. " +
                "This combination is not currently supported.");
        }

        if (instanceIdIndex >= 0)
        {
            RuntimeAPIParameter instanceId = api.Parameters[instanceIdIndex];

            if (instanceIdIndex != 0 ||
                instanceId.Type != "u64" ||
                instanceId.Direction != "in")
            {
                errors.Add(
                    $"API '{apiName}' parameter 'instanceId' must be " +
                    $"the first parameter with type 'u64' and direction 'in'.");
            }
        }
    }

    private static string GetDisplayName(RuntimeAPIDefinition api)
    {
        string module = string.IsNullOrWhiteSpace(api.Module)
            ? "<missing-module>"
            : api.Module;

        string name = string.IsNullOrWhiteSpace(api.Name)
            ? "<missing-name>"
            : api.Name;

        return $"{module}.{name}";
    }
}
