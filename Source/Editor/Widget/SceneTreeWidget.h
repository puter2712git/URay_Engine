#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class SelectionSystem;

class Engine;
class Unit;

class SceneTreeWidget final : public Widget
{
public:
    SceneTreeWidget(SelectionSystem& selectionSystem, Engine& engine);
    ~SceneTreeWidget() override;

public:
    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnDraw() override;

private:
    void DrawUnit(Unit* unit, Unit* currSelectedUnit);
    void DrawUnitDragDrop(Unit* unit);

private:
    SelectionSystem& selectionSystem;
    Engine& engine;

    Unit* pendingDraggedUnit = nullptr;
    Unit* pendingParentUnit = nullptr;
};

} // namespace URay
