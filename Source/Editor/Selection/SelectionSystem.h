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

    const std::vector<Unit*>& GetSelectedUnits() const { return selectedUnits; }
    Unit* GetActiveUnit() const { return activeUnit; }

private:
    void OnUnitRemoved(Scene* scene, Unit* unit);
    void NotifySelectionChanged(Unit* previousActive);

public:
    EventRay<> onSelectionSetChanged;
    EventRay<Unit*, Unit*> onActiveSelectionChanged;

private:
    std::vector<Unit*> selectedUnits;
    Unit* activeUnit = nullptr;
};

} // namespace URay
