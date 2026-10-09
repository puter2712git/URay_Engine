#pragma once

#include <Editor/Widget/Panel/Panel.h>

namespace URay
{

class SceneSystem;
class VerticalLayout;

class SceneTreePanel final : public Panel
{
    URAY_TYPE(SceneTreePanel, Panel)

public:
    SceneTreePanel();
    ~SceneTreePanel() override;

public:
    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    SceneSystem* sceneSystem = nullptr;

    VerticalLayout* layout = nullptr;

    bool isTreeDirty = false;
};

} // namespace URay
