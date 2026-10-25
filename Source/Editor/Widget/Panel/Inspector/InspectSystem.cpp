#include "InspectSystem.h"

#include <Engine/Scene/SceneSystem.h>

namespace URay
{

InspectSystem::InspectSystem(SceneSystem& sceneSystem) : sceneSystem(sceneSystem) {}

bool InspectSystem::Initialize()
{
    sceneSystem.onComponentAdded.Register(
        this, [this](Scene*, Unit* unit, Component*)
        { 
            if (IsInspecting(unit))
            {
                onTargetContentChanged.Emit(target);
            } });

    sceneSystem.onComponentDestroyed.Register(
        this, [this](Scene*, Unit* unit, Component*)
        { 
            if (IsInspecting(unit))
            {
                onTargetContentChanged.Emit(target);
            } });
    sceneSystem.onComponentPropertyChanged.Register(
        this, [this](Scene*, Unit* unit, Component*, const Property&)
        { 
            if (IsInspecting(unit))
            {
                onTargetContentChanged.Emit(target);
            } });
    sceneSystem.onUnitWorldTransformUpdated.Register(
        this, [this](Scene*, Unit* unit)
        {
            if (IsInspecting(unit)){
                onTargetContentChanged.Emit(target);
            } });
    sceneSystem.onUnitRemoved.Register(
        this, [this](Scene*, Unit* unit)
        { 
            if (IsInspecting(unit))
            {
                ClearTarget();
            } });

    return true;
}

void InspectSystem::Finalize()
{
    sceneSystem.onUnitRemoved.UnregisterAll(this);
    sceneSystem.onUnitWorldTransformUpdated.UnregisterAll(this);
    sceneSystem.onComponentPropertyChanged.UnregisterAll(this);
    sceneSystem.onComponentDestroyed.UnregisterAll(this);
    sceneSystem.onComponentAdded.UnregisterAll(this);

    ClearTarget();
}

void InspectSystem::SetTarget(InspectTarget newTarget)
{
    InspectTarget previousTarget = target;
    target = std::move(newTarget);

    onTargetChanged.Emit(previousTarget, target);
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

} // namespace URay
