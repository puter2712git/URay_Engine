#include "SceneTreePanel.h"

#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/SceneTree/SceneTreeNode.h>

#include <Engine/Engine.h>
#include <Engine/Scene/SceneSystem.h>

namespace URay
{

SceneTreePanel::SceneTreePanel() : Panel("Scene Tree")
{
    sceneSystem = &gEngine->GetSceneSystem();

    layout = AddChild<VerticalLayout>();

    sceneSystem->GetUnitAddRay().Register(this, [this](Scene*, Unit*)
                                          { isTreeDirty = true; });
    sceneSystem->GetUnitRemoveRay().Register(this, [this](Scene*, Unit*)
                                             { isTreeDirty = true; });
}

SceneTreePanel::~SceneTreePanel()
{
    sceneSystem->GetUnitAddRay().UnregisterAll(this);
    sceneSystem->GetUnitRemoveRay().UnregisterAll(this);
}

void SceneTreePanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);

    const Rect contentRect = GetContentRect();
    layout->Arrange(contentRect);
}

void SceneTreePanel::OnUpdate(float deltaTime)
{
    if (!isTreeDirty)
        return;

    isTreeDirty = false;

    layout->ClearChildren();

    Scene* gameScene = sceneSystem->GetSceneByType(SceneType::Game);
    if (!gameScene)
        return;

    for (const auto& unit : gameScene->GetUnits())
    {
        layout->AddChild<SceneTreeNode>(*unit, 0);
    }
}

} // namespace URay
