#pragma once

#include "Engine/Ray/EventRay.h"

namespace URay
{

class Unit;

namespace Render
{
class DrawCommandBuilder;
}

class SelectionSystem
{
public:
    SelectionSystem();
    ~SelectionSystem();

public:
    void PrepareRender(Render::DrawCommandBuilder& builder);

    void SelectUnit(Unit* unit);

    EventRay<Unit*>& GetOnSelectRay() { return onSelectRay; }
    Unit* GetSelectedUnit() const { return selectedUnit; }

private:
    EventRay<Unit*> onSelectRay;

    Unit* selectedUnit = nullptr;
};

} // namespace URay
