#include "ScriptComponent.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Engine.h>
#include <Engine/Script/ScriptSystem.h>

namespace URay
{

URAY_REGISTER_CLASS(ScriptComponent)
URAY_REGISTER_COMPONENT(ScriptComponent)

void ScriptComponent::RegisterClass()
{
}

void ScriptComponent::OnLoad()
{
    updateFailed = false;

    instanceHandle = gEngine->GetScriptSystem().CreateScript("URay.HelloScript");

    if (instanceHandle == 0)
    {
        URAY_LOG("[ScriptComponent] Failed to create HelloScript.");
    }
}

void ScriptComponent::OnUpdate(float deltaTime)
{
    if (instanceHandle == 0 || updateFailed)
        return;

    const int32 result = gEngine->GetScriptSystem().UpdateScript(instanceHandle, deltaTime);

    if (result != 0)
    {
        URAY_LOG("[ScriptComponent] Script update failed: %d", result);
        updateFailed = true;
    }
}

void ScriptComponent::OnUnload()
{
    if (instanceHandle == 0)
        return;

    const int32 handle = instanceHandle;
    instanceHandle = 0;

    const int32 result = gEngine->GetScriptSystem().UnloadScript(handle);

    if (result != 0)
    {
        URAY_LOG("[ScriptComponent] Script unload failed: %d", result);
    }
}

} // namespace URay
