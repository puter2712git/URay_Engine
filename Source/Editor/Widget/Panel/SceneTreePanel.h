#pragma once

#include "Editor/Widget/Panel/Panel.h"

namespace URay
{

class SelectionSystem;

class Engine;
class Scene;
class Unit;

class SceneTreePanel final : public Panel
{
    URAY_TYPE(SceneTreePanel, Panel)

public:
    SceneTreePanel();
    ~SceneTreePanel() override;

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
