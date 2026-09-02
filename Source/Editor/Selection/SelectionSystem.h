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

    RayHandle RegisterOnSelected(EventRay<Unit*>::Callback callback);
    void UnregisterOnSelected(RayHandle handle);

    Unit* GetSelectedUnit() const { return selectedUnit; }

private:
    EventRay<Unit*> onSelectRay;

    Unit* selectedUnit = nullptr;
};

} // namespace URay
