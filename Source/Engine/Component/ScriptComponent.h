#pragma once

#include <Engine/Asset/Asset.h>
#include <Engine/Component/Component.h>
#include <Engine/Script/ScriptMetadata.h>

#include <string>
#include <vector>

namespace URay
{

class ScriptAsset;

class ScriptComponent final : public Component
{
    URAY_CLASS(ScriptComponent, Component)

public:
    YAML::Node Serialize() const override;
    void Deserialize(const YAML::Node& node) override;

    const AssetHandle& GetScriptHandle() const { return scriptHandle; }
    ScriptAsset* GetScript() const;

    void SetScript(AssetHandle handle);

    const std::vector<ScriptFieldValue>& GetFields() const { return fields; }
    const ScriptFieldValue* FindField(const std::string& name) const;
    bool SetFieldValue(const std::string& name, const ScriptValue& value);

    void RefreshFieldsFromSchema();

private:
    AssetHandle scriptHandle = {};
    std::vector<ScriptFieldValue> fields;
};

} // namespace URay