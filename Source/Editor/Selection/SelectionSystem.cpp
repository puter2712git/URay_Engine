#include "SelectionSystem.h"

#include <Engine/Engine.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

#include <algorithm>

namespace URay
{

SelectionSystem::SelectionSystem()
{
    URAY_DECLARE_RAY(SelectionSetChanged);
    URAY_DECLARE_RAY(ActiveSelectionChanged, Unit*, Unit*);
}

SelectionSystem::~SelectionSystem() = default;

bool SelectionSystem::Initialize()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    sceneSystem.Register("UnitRemoved", this, &SelectionSystem::OnUnitRemoved);

    return true;
}

void SelectionSystem::Finalize()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    sceneSystem.UnregisterAll(this);
}

void SelectionSystem::SetSelection(Unit* unit)
{
    const bool alreadyOnlySelected =
        selectedUnits.size() == (unit ? 1u : 0u) &&
        (!unit || selectedUnits.front() == unit) &&
        activeUnit == unit;

    if (alreadyOnlySelected)
        return;

    Unit* previousActive = activeUnit;

    selectedUnits.clear();

    if (unit)
        selectedUnits.push_back(unit);

    activeUnit = unit;

    NotifySelectionChanged(previousActive);
}

void SelectionSystem::ToggleSelection(Unit* unit)
{
    if (!unit)
        return;

    const auto it = std::find(selectedUnits.begin(), selectedUnits.end(), unit);

    Unit* previousActive = activeUnit;

    if (it == selectedUnits.end())
    {
        selectedUnits.push_back(unit);
        activeUnit = unit;
    }
    else
    {
        selectedUnits.erase(it);

        if (activeUnit == unit)
        {
            activeUnit = selectedUnits.empty()
                             ? nullptr
                             : selectedUnits.back();
        }
    }

    NotifySelectionChanged(previousActive);
}

void SelectionSystem::ClearSelection()
{
    if (selectedUnits.empty() && !activeUnit)
        return;

    Unit* previousActive = activeUnit;

    selectedUnits.clear();
    activeUnit = nullptr;

    NotifySelectionChanged(previousActive);
}

bool SelectionSystem::IsSelected(const Unit* unit) const
{
    const auto it = std::find(selectedUnits.begin(), selectedUnits.end(), unit);
    return it != selectedUnits.end();
}

void SelectionSystem::OnUnitRemoved(Scene* scene, Unit* unit)
{
    const auto it = std::find(selectedUnits.begin(), selectedUnits.end(), unit);

    if (it == selectedUnits.end())
        return;

    Unit* previousActive = activeUnit;

    selectedUnits.erase(it);

    if (activeUnit == unit)
    {
        activeUnit = selectedUnits.empty()
                         ? nullptr
                         : selectedUnits.back();
    }

    NotifySelectionChanged(previousActive);
}

void SelectionSystem::NotifySelectionChanged(Unit* previousActive)
{
    Emit("SelectionSetChanged");

    if (previousActive != activeUnit)
    {
        Emit("ActiveSelectionChanged", previousActive, activeUnit);
    }
}

} // namespace URay
