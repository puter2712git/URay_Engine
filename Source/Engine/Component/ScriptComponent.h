#pragma once

#include <Engine/Component/Component.h>
#include <Engine/Script/ScriptMetadata.h>

#include <string>
#include <vector>

namespace URay
{

class ScriptComponent final : public Component
{
    URAY_CLASS(ScriptComponent, Component)

public:
    YAML::Node Serialize() const override;
    void Deserialize(const YAML::Node& node) override;

    const std::string& GetScriptClassName() const { return scriptClassName; }
    void SetScriptClassName(std::string className);

    const std::vector<ScriptFieldValue>& GetFields() const { return fields; }
    const ScriptFieldValue* FindField(const std::string& name) const;
    bool SetFieldValue(const std::string& name, const ScriptValue& value);

    void RefreshFieldsFromSchema();

private:
    std::string scriptClassName;
    std::vector<ScriptFieldValue> fields;
};

} // namespace URay