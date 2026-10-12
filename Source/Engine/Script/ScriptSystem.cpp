#include "ScriptSystem.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Script/NativeAPI.h>

#include <algorithm>
#include <cstring>
#include <string>
#include <utility>

namespace URay
{

bool ScriptSystem::Initialize(const std::filesystem::path& projectPath)
{
    const std::filesystem::path managedOutput =
        projectPath / "Managed" / "TestManaged" / "bin" / "Debug" / "net10.0";

    const std::filesystem::path runtimeConfigPath = managedOutput / "URay.runtimeconfig.json";

    const std::filesystem::path assemblyPath = managedOutput / "URay.dll";

    if (!dotnetHost.Initialize(runtimeConfigPath, assemblyPath))
        return false;

    NativeAPI nativeAPI = {
        .userData = this,
        .LogUTF8 = &ScriptSystem::LogUTF8,
        .RegisterScriptType = &ScriptSystem::RegisterScriptTypeUTF8,
        .RegisterScriptField = &ScriptSystem::RegisterScriptFieldUTF8
    };

    if (!dotnetHost.InitializeManagedAPI(nativeAPI))
        return false;

    isInitialized = true;
    return true;
}

void ScriptSystem::Finalize()
{
    scriptTypes.clear();
    isInitialized = false;
}

bool ScriptSystem::RunSample()
{
    if (!isInitialized)
        return false;

    return dotnetHost.RunSample();
}

bool ScriptSystem::RegisterScriptType(ScriptTypeDefinition definition)
{
    if (definition.className.empty())
        return false;

    const std::string className = definition.className;

    scriptTypes.insert_or_assign(className, std::move(definition));
    return true;
}

const ScriptTypeDefinition* ScriptSystem::FindScriptType(const std::string& className) const
{
    const auto it = scriptTypes.find(className);
    return it != scriptTypes.end() ? &it->second : nullptr;
}

void ScriptSystem::LogUTF8(void* userData, const uint8* message, int32 byteCount)
{
    if (message == nullptr || byteCount < 0)
        return;

    const char* text = reinterpret_cast<const char*>(message);
    URAY_LOG("%s", std::string(text, byteCount).c_str());
}

int32 ScriptSystem::RegisterScriptTypeUTF8(
    void* userData,
    const uint8* className,
    int32 classNameLength)
{
    if (!userData || !className || classNameLength <= 0)
        return 0;

    auto& scriptSystem = *static_cast<ScriptSystem*>(userData);

    ScriptTypeDefinition definition;
    definition.className = std::string(
        reinterpret_cast<const char*>(className),
        classNameLength);

    return scriptSystem.RegisterScriptType(std::move(definition)) ? 1 : 0;
}

int32 ScriptSystem::RegisterScriptFieldUTF8(
    void* userData,
    const uint8* className,
    int32 classNameLength,
    const uint8* fieldName,
    int32 fieldNameLength,
    int32 fieldType,
    const void* defaultValue,
    int32 defaultValueSize)
{
    if (!userData || !className || classNameLength <= 0 ||
        !fieldName || fieldNameLength <= 0 ||
        defaultValueSize < 0)
    {
        return 0;
    }

    auto& scriptSystem = *static_cast<ScriptSystem*>(userData);

    const std::string classNameString(reinterpret_cast<const char*>(className), classNameLength);
    const std::string fieldNameString(reinterpret_cast<const char*>(fieldName), fieldNameLength);

    return scriptSystem.RegisterScriptField(
               classNameString,
               fieldNameString,
               static_cast<ScriptFieldType>(fieldType),
               defaultValue,
               defaultValueSize)
               ? 1
               : 0;
}

bool ScriptSystem::RegisterScriptField(
    const std::string& className,
    const std::string& fieldName,
    ScriptFieldType fieldType,
    const void* defaultValue,
    int32 defaultValueSize)
{
    const auto it = scriptTypes.find(className);
    if (it == scriptTypes.end())
        return false;

    if (fieldName.empty())
        return false;

    ScriptTypeDefinition& scriptType = it->second;

    const auto duplicateIt = std::find_if(
        scriptType.fields.begin(),
        scriptType.fields.end(),
        [&fieldName](const ScriptFieldDefinition& field)
        {
            return field.name == fieldName;
        });

    if (duplicateIt != scriptType.fields.end())
        return false;

    ScriptFieldDefinition definition;
    definition.name = fieldName;

    switch (fieldType)
    {
    case ScriptFieldType::Bool:
    {
        if (!defaultValue || defaultValueSize != sizeof(uint8))
            return false;

        const uint8 value = *static_cast<const uint8*>(defaultValue);

        definition.type = PropertyType::Bool;
        definition.defaultValue = value != 0;
        break;
    }

    case ScriptFieldType::Int:
    {
        if (!defaultValue || defaultValueSize != sizeof(int32))
            return false;

        int32 value = 0;
        std::memcpy(&value, defaultValue, sizeof(value));

        definition.type = PropertyType::Int;
        definition.defaultValue = value;
        break;
    }

    case ScriptFieldType::Float:
    {
        if (!defaultValue || defaultValueSize != sizeof(float))
            return false;

        float value = 0.0f;
        std::memcpy(&value, defaultValue, sizeof(value));

        definition.type = PropertyType::Float;
        definition.defaultValue = value;
        break;
    }

    case ScriptFieldType::Vector2:
    {
        if (!defaultValue || defaultValueSize != sizeof(Vector2))
            return false;

        Vector2 value = {};
        std::memcpy(&value, defaultValue, sizeof(value));

        definition.type = PropertyType::Vector2;
        definition.defaultValue = value;
        break;
    }

    case ScriptFieldType::Vector3:
    {
        if (!defaultValue || defaultValueSize != sizeof(Vector3))
            return false;

        Vector3 value = {};
        std::memcpy(&value, defaultValue, sizeof(value));

        definition.type = PropertyType::Vector3;
        definition.defaultValue = value;
        break;
    }

    case ScriptFieldType::Vector4:
    {
        if (!defaultValue || defaultValueSize != sizeof(Vector4))
            return false;

        Vector4 value = {};
        std::memcpy(&value, defaultValue, sizeof(value));

        definition.type = PropertyType::Vector4;
        definition.defaultValue = value;
        break;
    }

    case ScriptFieldType::Color:
    {
        if (!defaultValue || defaultValueSize != sizeof(Color))
            return false;

        Color value = {};
        std::memcpy(&value, defaultValue, sizeof(value));

        definition.type = PropertyType::Color;
        definition.defaultValue = value;
        break;
    }

    case ScriptFieldType::String:
    {
        if (defaultValueSize > 0 && !defaultValue)
            return false;

        const char* value =
            static_cast<const char*>(defaultValue);

        definition.type = PropertyType::String;
        definition.defaultValue = std::string(
            value ? value : "",
            static_cast<size_t>(defaultValueSize));
        break;
    }

    default:
        return false;
    }

    scriptType.fields.push_back(std::move(definition));
    return true;
}

} // namespace URay
