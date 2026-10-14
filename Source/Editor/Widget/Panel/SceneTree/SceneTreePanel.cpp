#include "SceneTreePanel.h"

#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/SceneTree/SceneTreeNode.h>
#include <Editor/Widget/Popup/UnitPopup.h>
#include <Editor/Widget/WidgetSystem.h>
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
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);

    sceneSystem->GetUnitAddRay().Register(this, [this](Scene*, Unit*)
                                          { isTreeDirty = true; });
    sceneSystem->GetUnitRemoveRay().Register(this, [this](Scene*, Unit*)
                                             { isTreeDirty = true; });

    UpdateTree();
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

EventReply SceneTreePanel::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Right)
        return {};

    PopupLayer& popupLayer = gEditor->GetWidgetSystem().GetPopupLayer();
    UnitPopup* popup = popupLayer.OpenPopup<UnitPopup>(event.position);

    popup->GetUnitSelectedRay().Register(
        popup,
        [this, &popupLayer](UnitPopupSelection selection)
        {
            if (selection != UnitPopupSelection::EmptyUnit)
                return;

            Scene* gameScene = sceneSystem->GetSceneByType(SceneType::Game);
            if (!gameScene)
            {
                popupLayer.ClosePopup();
                return;
            }

            Unit* emptyUnit = gameScene->CreateEmptyUnit("Empty Unit", nullptr);

            gEditor->GetSelectionSystem().SetSelection(emptyUnit);

            popupLayer.ClosePopup();
        });

    return EventReply{
        .handled = true,
        .requestFocus = true
    };
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
