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

SceneTreeNode::SceneTreeNode(Unit& unit, uint32 depth)
    : unit(unit), depth(depth)
{
    selectionSystem = &gEditor->GetSelectionSystem();

    SetPreferredHeight(20.0f);

    surface = AddChild<Surface>();
    surface->SetBackgroundColor(Color::Black);
    surface->SetPadding(Padding(5.0f, 2.0f));

    layout = surface->AddChild<HorizontalLayout>();
    layout->SetPadding(Padding(depth * 16.0f, 0));

    label = layout->AddChild<Label>(unit.GetName());
    label->SetTextStyle(EditorStyle::SmallText());
}

void SceneTreeNode::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    surface->Arrange(rect);
}

EventReply SceneTreeNode::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Right)
    {
        PopupLayer& popupLayer = gEditor->GetWidgetSystem().GetPopupLayer();
        UnitPopup* popup = popupLayer.OpenPopup<UnitPopup>(event.position);

        popup->GetUnitSelectedRay().Register(
            popup,
            [this, &popupLayer](UnitPopupSelection selection)
            {
                if (selection != UnitPopupSelection::EmptyUnit)
                    return;

                Scene* scene = unit.GetOwner();

                if (!scene)
                {
                    popupLayer.ClosePopup();
                    return;
                }

                Unit* createdUnit = scene->CreateEmptyUnit(
                    "New Unit",
                    &unit);

                selectionSystem->SetSelection(createdUnit);

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
            selectionSystem->ToggleSelection(&unit);
        }
        else
        {
            selectionSystem->SetSelection(&unit);
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
    Color surfaceColor = EditorStyle::Surface;

    if (selectionSystem->IsSelected(&unit))
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
