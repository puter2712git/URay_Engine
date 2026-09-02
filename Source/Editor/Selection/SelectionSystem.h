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

    RayHandle RegisterOnSelected(EventRay<Unit*>::Callback callback);
    void UnregisterOnSelected(RayHandle handle);

    Unit* GetSelectedUnit() const { return selectedUnit; }

private:
    EventRay<Unit*> onSelectRay;

    Unit* selectedUnit = nullptr;
};

} // namespace URay
