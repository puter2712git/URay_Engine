#include "SceneTreeNode.h"

#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Element/Label.h>
#include <Editor/Widget/Element/Surface.h>
#include <Editor/Widget/Layout/HorizontalLayout.h>

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
    selectionSystem->SelectUnit(&unit);

    return EventReply{
        .handled = true
    };
}

void SceneTreeNode::OnUpdate(float deltaTime)
{
    Color surfaceColor = EditorStyle::Surface;

    if (IsHovered())
    {
        surfaceColor = EditorStyle::SurfaceHeader;
    }

    surface->SetBackgroundColor(surfaceColor);
}

} // namespace URay
