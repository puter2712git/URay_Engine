#include "SceneTreeNode.h"

#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Element/Label.h>
#include <Editor/Widget/Element/Surface.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Layout/HorizontalLayout.h>
#include <Editor/Widget/Popup/UnitPopup.h>
#include <Editor/Widget/WidgetSystem.h>
#include <Engine/Scene/Scene.h>

#include <Engine/Scene/Unit.h>

namespace URay
{

SceneTreeNode::SceneTreeNode()
{
    selectionSystem = &gEditor->GetSelectionSystem();

    SetPreferredHeight(20.0f);

    surface = AddChild<Surface>();
    surface->SetPadding(Padding(5.0f, 2.0f));

    layout = surface->AddChild<HorizontalLayout>();
    label = layout->AddChild<Label>();
    label->SetTextStyle(EditorStyle::SmallText());
}

void SceneTreeNode::Bind(Unit& unit, uint32 depth)
{
    this->unit = &unit;
    this->depth = depth;

    label->SetText(unit.GetName());
    layout->SetPadding(Padding(depth * 16.0f, 0.0f));
}

void SceneTreeNode::Unbind()
{
    unit = nullptr;
    depth = 0;
    label->SetText({});
    surface->SetBackgroundColor(EditorStyle::Surface);
}

void SceneTreeNode::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    surface->Arrange(rect);
}

EventReply SceneTreeNode::OnPointerDown(const PointerEvent& event)
{
    if (!unit)
        return {};

    if (event.changedButton == MouseButton::Right)
    {
        PopupLayer& popupLayer = gEditor->GetWidgetSystem().GetPopupLayer();
        UnitPopup* popup = popupLayer.OpenPopup<UnitPopup>(event.position);

        Unit* targetUnit = unit;
        SelectionSystem* targetSelectionSystem = selectionSystem;

        popup->GetUnitSelectedRay().Register(
            popup,
            [targetUnit, targetSelectionSystem, &popupLayer](UnitPopupSelection selection)
            {
                if (selection != UnitPopupSelection::EmptyUnit)
                    return;

                Scene* scene = targetUnit->GetOwner();

                if (!scene)
                {
                    popupLayer.ClosePopup();
                    return;
                }

                Unit* createdUnit = scene->CreateEmptyUnit(
                    "New Unit",
                    targetUnit);

                targetSelectionSystem->SetSelection(createdUnit);

                popupLayer.ClosePopup();
            });

        return EventReply{
            .handled = true,
            .requestFocus = true
        };
    }

    if (event.changedButton == MouseButton::Left)
    {
        const bool isToggleSelection = (event.modifiers & ModifierKey::Ctrl) != ModifierKey::None;

        if (isToggleSelection)
        {
            selectionSystem->ToggleSelection(unit);
        }
        else
        {
            selectionSystem->SetSelection(unit);
        }

        return EventReply{
            .handled = true,
            .requestFocus = true
        };
    }

    return {};
}

void SceneTreeNode::OnUpdate(float deltaTime)
{
    if (!unit)
        return;

    Color surfaceColor = EditorStyle::Surface;

    if (selectionSystem->IsSelected(unit))
    {
        surfaceColor = EditorStyle::ControlPressed;
    }
    else if (IsHovered())
    {
        surfaceColor = EditorStyle::ControlHovered;
    }

    surface->SetBackgroundColor(surfaceColor);
}

} // namespace URay
