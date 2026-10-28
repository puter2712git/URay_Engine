#include "InspectSystem.h"

#include <Engine/Component/Component.h>
#include <Engine/Object/Property/Property.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

namespace URay
{

InspectSystem::InspectSystem(SceneSystem& sceneSystem) : sceneSystem(sceneSystem)
{
    URAY_DECLARE_RAY(TargetChanged, const InspectTarget*, const InspectTarget*);
    URAY_DECLARE_RAY(TargetContentChanged, const InspectTarget*);
}

bool InspectSystem::Initialize()
{
    sceneSystem.Register("ComponentAdded", this, &InspectSystem::OnComponentChanged);

    sceneSystem.Register("ComponentDestroyed", this, &InspectSystem::OnComponentChanged);
    sceneSystem.Register("ComponentPropertyChanged", this, &InspectSystem::OnComponentPropertyChanged);
    sceneSystem.Register("UnitRemoved", this, &InspectSystem::OnUnitRemoved);
    sceneSystem.Register("UnitWorldTransformUpdated", this, &InspectSystem::OnUnitWorldTransformUpdated);

    return true;
}

void InspectSystem::Finalize()
{
    sceneSystem.UnregisterAll(this);

    ClearTarget();
}

void InspectSystem::SetTarget(InspectTarget newTarget)
{
    InspectTarget previousTarget = target;
    target = std::move(newTarget);

    Emit("TargetChanged", static_cast<const InspectTarget*>(&previousTarget), static_cast<const InspectTarget*>(&target));
}

void InspectSystem::ClearTarget()
{
    if (target.IsEmpty())
        return;

    SetTarget({});
}

Unit* InspectSystem::GetInspectedUnit() const
{
    const auto* unitTarget = std::get_if<UnitInspectTarget>(&target.data);
    if (!unitTarget)
        return nullptr;

    return unitTarget->unit;
}

bool InspectSystem::IsInspecting(const Unit* unit) const
{
    const auto* unitTarget = std::get_if<UnitInspectTarget>(&target.data);
    if (!unitTarget)
        return false;

    return unitTarget->unit == unit;
}

void InspectSystem::OnComponentChanged(Scene*, Unit* unit, Component*)
{
    if (IsInspecting(unit))
        Emit("TargetContentChanged", static_cast<const InspectTarget*>(&target));
}

void InspectSystem::OnComponentPropertyChanged(Scene* scene, Unit* unit, Component* component, const Property*)
{
    OnComponentChanged(scene, unit, component);
}

void InspectSystem::OnUnitRemoved(Scene* scene, Unit* unit)
{
    if (IsInspecting(unit))
    {
        ClearTarget();
    }
}

void InspectSystem::OnUnitWorldTransformUpdated(Scene* scene, Unit* unit)
{
    if (IsInspecting(unit))
    {
        Emit("TargetContentChanged", static_cast<const InspectTarget*>(&target));
    }
}

} // namespace URay
