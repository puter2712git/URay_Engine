#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/Component/Component.h>

namespace URay
{

class ScriptComponent : public Component
{
    URAY_OBJECT(ScriptComponent, Component)

public:
    const String& GetScriptTypeName() const;
    void SetScriptTypeName(const String& typeName);

protected:
    void OnLoad() override;
    void OnUpdate(float deltaTime) override;
    void OnUnload() override;

private:
    String scriptTypeName;

    int32 instanceHandle = 0;
    bool updateFailed = false;
};

} // namespace URay
