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
    void DrawUnit(Unit* unit, Unit* currSelectedUnit) const;

private:
    Engine& engine;
    Editor& editor;
};

} // namespace URay
