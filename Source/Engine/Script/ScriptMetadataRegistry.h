#pragma once

#include <Engine/Script/ScriptMetadata.h>

#include <string>
#include <unordered_map>

namespace URay
{

class AssetDatabase;
class ScriptAsset;

class ScriptMetadataRegistry
{
public:
    explicit ScriptMetadataRegistry(AssetDatabase& assetDatabase);

    bool RegisterType(ScriptTypeDefinition definition);

    bool RegisterField(
        const std::string& className,
        const std::string& fieldName,
        ScriptFieldType fieldType,
        const void* defaultValue,
        int32 defaultValueSize);

    const ScriptTypeDefinition* FindType(const std::string& className) const;

    void Clear();

private:
    ScriptAsset* FindScriptAssetByClassName(const std::string& className) const;
    bool SyncScriptAsset(const ScriptTypeDefinition& definition) const;

private:
    AssetDatabase& assetDatabase;
    std::unordered_map<std::string, ScriptTypeDefinition> scriptTypes;
};

} // namespace URay
