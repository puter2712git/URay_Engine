#include "LifecycleTestComponent.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Scene/Unit.h>

namespace URay
{

URAY_REGISTER_CLASS(LifecycleTestComponent)
URAY_REGISTER_COMPONENT(LifecycleTestComponent)

void LifecycleTestComponent::RegisterClass() {}

void LifecycleTestComponent::OnInitialize()
{
    URAY_LOG("[Lifecycle] %s Initialize", GetOwner()->GetName().c_str());
}

void LifecycleTestComponent::OnLoad()
{
    URAY_LOG("[Lifecycle] %s Load", GetOwner()->GetName().c_str());
}

void LifecycleTestComponent::OnUpdate(float deltaTime)
{
    URAY_LOG("[Lifecycle] %s Update", GetOwner()->GetName().c_str());
}

void LifecycleTestComponent::OnUnload()
{
    URAY_LOG("[Lifecycle] %s Unload", GetOwner()->GetName().c_str());
}

void LifecycleTestComponent::OnFinalize()
{
    URAY_LOG("[Lifecycle] %s Finalize", GetOwner()->GetName().c_str());
}

} // namespace URay
