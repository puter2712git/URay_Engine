#include "TransformBindings.h"

#include <Engine/Component/TransformComponent.h>
#include <Engine/Script/ScriptSystem.h>

namespace URay::ScriptBindings
{

bool GetTransformPosition(
    ScriptSystem& scriptSystem,
    uint64 instanceId,
    Vector3& position)
{
    TransformComponent* transform = scriptSystem.FindTransformForManagedInstance(instanceId);

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
    TransformComponent* transform = scriptSystem.FindTransformForManagedInstance(instanceId);

    if (!transform)
        return false;

    transform->SetPosition(position);
    return true;
}

} // namespace URay::ScriptBindings
