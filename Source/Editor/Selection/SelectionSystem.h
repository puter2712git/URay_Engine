#pragma once

#include "Engine/Ray/EventRay.h"

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

    void SelectUnit(Unit* unit);

    EventRay<Unit*, Unit*>& GetOnSelectionChangedRay() { return onSelectionChangedRay; }
    Unit* GetSelectedUnit() const { return selectedUnit; }

private:
    void OnUnitRemoved(Scene* scene, Unit* unit);

private:
    EventRay<Unit*, Unit*> onSelectionChangedRay;

    Unit* selectedUnit = nullptr;
};

} // namespace URay
