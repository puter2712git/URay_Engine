#include "SelectionSystem.h"

#include "Engine/Engine.h"
#include "Engine/Scene/SceneSystem.h"

namespace URay
{

SelectionSystem::SelectionSystem() = default;

SelectionSystem::~SelectionSystem() = default;

bool SelectionSystem::Initialize()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    sceneSystem.GetUnitRemoveRay().Register(
        this,
        [this](Scene* scene, Unit* unit)
        { OnUnitRemoved(scene, unit); });

    return true;
}

void SelectionSystem::Finalize()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    sceneSystem.GetUnitRemoveRay().UnregisterAll(this);
}

void SelectionSystem::SelectUnit(Unit* unit)
{
    Unit* previousUnit = selectedUnit;
    selectedUnit = unit;
    onSelectionChangedRay.Emit(previousUnit, selectedUnit);
}

void SelectionSystem::OnUnitRemoved(Scene* scene, Unit* unit)
{
    if (unit == selectedUnit)
    {
        SelectUnit(nullptr);
    }
}

} // namespace URay
