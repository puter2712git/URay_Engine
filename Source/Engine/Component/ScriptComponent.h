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

    friend class ScriptSystem;

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

protected:
    void OnLoad() override;
    void OnUpdate(float deltaTime) override;
    void OnUnload() override;
    void OnFinalize() override;

private:
    void ReleaseManagedInstance();
    bool CreateManagedInstance();

private:
    AssetHandle scriptHandle = {};
    std::vector<ScriptFieldValue> fields;

    uint64 managedInstanceId = 0;
};

} // namespace URay