#pragma once

#include "Editor/Widget/Panel/Panel.h"

namespace URay
{

class SelectionSystem;

class Engine;
class Scene;
class Unit;

class SceneTreeWidget final : public Panel
{
    URAY_TYPE(SceneTreeWidget, Panel)

public:
    SceneTreeWidget();
    ~SceneTreeWidget() override;

public:
    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnDraw() override;

private:
    void DrawScene(Scene& scene, Unit* currSelectedUnit);
    void DrawUnit(Unit* unit, Unit* currSelectedUnit);
    void DrawUnitDragDrop(Unit* unit);

private:
    SelectionSystem& selectionSystem;

    Unit* pendingDraggedUnit = nullptr;
    Unit* pendingParentUnit = nullptr;
};

} // namespace URay
