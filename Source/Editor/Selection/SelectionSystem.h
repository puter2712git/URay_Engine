#pragma once

#include <Core/Ray/EventRay.h>

#include <vector>

namespace URay
{

class Scene;
class Unit;

class SelectionSystem
{
public:
    SelectionSystem();
    ~SelectionSystem();

public:
    bool Initialize();
    void Finalize();

    void SetSelection(Unit* unit);
    void ToggleSelection(Unit* unit);
    void ClearSelection();

    bool IsSelected(const Unit* unit) const;

    EventRay<>& GetOnSelectionSetChangedRay() { return onSelectionSetChanged; }
    EventRay<Unit*, Unit*>& GetOnActiveSelectionChangedRay() { return onActiveSelectionChanged; }

    const std::vector<Unit*>& GetSelectedUnits() const { return selectedUnits; }
    Unit* GetActiveUnit() const { return activeUnit; }

private:
    void OnUnitRemoved(Scene* scene, Unit* unit);
    void NotifySelectionChanged(Unit* previousActive);

private:
    EventRay<> onSelectionSetChanged;
    EventRay<Unit*, Unit*> onActiveSelectionChanged;

    std::vector<Unit*> selectedUnits;
    Unit* activeUnit = nullptr;
};

} // namespace URay
