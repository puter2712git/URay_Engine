#pragma once

#include <Editor/Widget/Panel/Panel.h>

namespace URay
{

class SceneSystem;
class VerticalLayout;
class Unit;

class SceneTreePanel final : public Panel
{
    URAY_TYPE(SceneTreePanel, Panel)

public:
    SceneTreePanel();
    ~SceneTreePanel() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    void UpdateTree();
    void AddSubtree(Unit& unit, uint32 depth);

private:
    SceneSystem* sceneSystem = nullptr;

    VerticalLayout* layout = nullptr;

    bool isTreeDirty = false;
};

} // namespace URay
