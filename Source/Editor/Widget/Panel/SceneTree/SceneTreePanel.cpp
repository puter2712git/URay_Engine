#include "SceneTreePanel.h"

#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/SceneTree/SceneTreeNode.h>

#include <Engine/Engine.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

namespace URay
{

SceneTreePanel::SceneTreePanel() : Panel("Scene Tree")
{
    sceneSystem = &gEngine->GetSceneSystem();

    SetBackgroundColor(EditorStyle::Surface);
    SetOutlineWidth(2.0f);
    SetOutlineColor(EditorStyle::Border);
    SetPadding(Padding(5.0f, 5.0f));

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

    UpdateTree();
}

void SceneTreePanel::UpdateTree()
{
    layout->ClearChildren();

    Scene* gameScene = sceneSystem->GetSceneByType(SceneType::Game);
    if (!gameScene)
        return;

    const auto& units = gameScene->GetUnits();

    for (const auto& unit : units)
    {
        if (!unit->GetParent())
            AddSubtree(*unit, 0);
    }
}

void SceneTreePanel::AddSubtree(Unit& unit, uint32 depth)
{
    layout->AddChild<SceneTreeNode>(unit, depth);

    for (Unit* child : unit.GetChildren())
    {
        if (child)
        {
            AddSubtree(*child, depth + 1);
        }
    }
}

} // namespace URay
