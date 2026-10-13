#include "TransformBindings.h"

#include <Engine/Component/ScriptComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Scene/Unit.h>
#include <Engine/Script/ScriptSystem.h>

namespace
{

URay::TransformComponent* FindTransform(
    URay::ScriptSystem& scriptSystem,
    URay::uint64 instanceId)
{
    URay::ScriptComponent* scriptComponent =
        scriptSystem.FindManagedInstance(instanceId);

    if (!scriptComponent)
        return nullptr;

    URay::Unit* unit = scriptComponent->GetOwner();
    return unit ? unit->GetTransform() : nullptr;
}

} // namespace

namespace URay::ScriptBindings
{

bool GetTransformPosition(
    ScriptSystem& scriptSystem,
    uint64 instanceId,
    Vector3& position)
{
    TransformComponent* transform = FindTransform(scriptSystem, instanceId);

    if (!transform)
        return false;

    position = transform->GetPosition();
    return true;
}

bool SetTransformPosition(
    ScriptSystem& scriptSystem,
    uint64 instanceId,
    const Vector3& position)
{
    TransformComponent* transform = FindTransform(scriptSystem, instanceId);

    if (!transform)
        return false;

    transform->SetPosition(position);
    return true;
}

} // namespace URay::ScriptBindings
