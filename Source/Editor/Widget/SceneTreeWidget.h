#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class Engine;
class Editor;
class Unit;

class SceneTreeWidget final : public Widget
{
public:
    SceneTreeWidget(Editor& editor, Engine& engine);
    ~SceneTreeWidget() override;

public:
    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnDraw() override;

private:
    void DrawUnit(Unit* unit, Unit* currSelectedUnit);
    void DrawUnitDragDrop(Unit* unit);

private:
    Engine& engine;
    Editor& editor;

    Unit* pendingDraggedUnit = nullptr;
    Unit* pendingParentUnit = nullptr;
};

} // namespace URay
