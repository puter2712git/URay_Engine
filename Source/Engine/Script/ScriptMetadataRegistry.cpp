#include "ScriptMetadataRegistry.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/Script/ScriptAsset.h>

#include <algorithm>
#include <cstring>
#include <string_view>
#include <utility>

namespace URay
{

ScriptMetadataRegistry::ScriptMetadataRegistry(AssetDatabase& assetDatabase)
    : assetDatabase(assetDatabase)
{
}

bool ScriptMetadataRegistry::RegisterType(ScriptTypeDefinition definition)
{
    if (definition.className.empty())
        return false;

    const std::string className = definition.className;

    auto [it, inserted] = scriptTypes.insert_or_assign(className, std::move(definition));

    return SyncScriptAsset(it->second);
}

bool ScriptMetadataRegistry::RegisterField(
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

        const char* value = static_cast<const char*>(defaultValue);

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

    return SyncScriptAsset(scriptType);
}

const ScriptTypeDefinition* ScriptMetadataRegistry::FindType(
    const std::string& className) const
{
    const auto it = scriptTypes.find(className);
    return it != scriptTypes.end() ? &it->second : nullptr;
}

void ScriptMetadataRegistry::Clear()
{
    scriptTypes.clear();
}

ScriptAsset* ScriptMetadataRegistry::FindScriptAssetByClassName(
    const std::string& className) const
{
    const size_t separator = className.rfind('.');

    const std::string_view simpleName =
        separator == std::string::npos
            ? std::string_view(className)
            : std::string_view(className).substr(separator + 1);

    ScriptAsset* found = nullptr;

    for (ScriptAsset* asset : assetDatabase.GetAssets<ScriptAsset>())
    {
        if (asset->GetName() != simpleName)
            continue;

        if (found)
        {
            URAY_LOG(
                "Multiple ScriptAssets match managed type: %s",
                className.c_str());

            return nullptr;
        }

        found = asset;
    }

    return found;
}

bool ScriptMetadataRegistry::SyncScriptAsset(
    const ScriptTypeDefinition& definition) const
{
    ScriptAsset* asset = FindScriptAssetByClassName(definition.className);

    if (!asset)
    {
        URAY_LOG(
            "ScriptAsset was not found for managed type: %s",
            definition.className.c_str());

        return false;
    }

    asset->SetManagedClassName(definition.className);
    asset->SetFields(definition.fields);

    return true;
}

} // namespace URay
