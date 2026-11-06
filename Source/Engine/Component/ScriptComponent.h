#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/Component/Component.h>
#include <Engine/Script/ScriptField.h>

namespace URay
{

class ScriptComponent : public Component
{
    URAY_OBJECT(ScriptComponent, Component)

public:
    const String& GetScriptTypeName() const;
    void SetScriptTypeName(const String& typeName);

    const Vector<ScriptFieldOverride>& GetFieldOverrides() const;
    const ScriptFieldOverride* GetFieldOverride(const ScriptFieldInfo& field) const;
    bool SetFieldOverride(const ScriptFieldInfo& field, std::optional<Variant> value);

    YAML::Node Serialize() const override;
    void Deserialize(const YAML::Node& node) override;

protected:
    void OnLoad() override;
    void OnUpdate(float deltaTime) override;
    void OnUnload() override;

private:
    String scriptTypeName;

    int32 instanceHandle = 0;
    bool updateFailed = false;

    Vector<ScriptFieldOverride> fieldOverrides;
};

} // namespace URay
