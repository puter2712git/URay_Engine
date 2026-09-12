#pragma once

#include "Engine/Ray/EventRay.h"

namespace URay
{

class Unit;

class SelectionSystem
{
public:
    SelectionSystem();
    ~SelectionSystem();

public:
    void SelectUnit(Unit* unit);

    EventRay<Unit*, Unit*>& GetOnSelectionChangedRay() { return onSelectionChangedRay; }
    Unit* GetSelectedUnit() const { return selectedUnit; }

private:
    EventRay<Unit*, Unit*> onSelectionChangedRay;

    Unit* selectedUnit = nullptr;
};

} // namespace URay
