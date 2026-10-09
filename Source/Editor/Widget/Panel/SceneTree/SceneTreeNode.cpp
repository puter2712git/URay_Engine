#include "SceneTreeNode.h"

#include <Editor/Widget/Element/Label.h>
#include <Editor/Widget/Layout/HorizontalLayout.h>

#include <Engine/Scene/Unit.h>

namespace URay
{

SceneTreeNode::SceneTreeNode(Unit& unit, uint32 depth)
    : unit(unit), depth(depth)
{
    SetDesiredSize(Vector2(100.0f, 20.0f));

    layout = AddChild<HorizontalLayout>();

    label = layout->AddChild<Label>(unit.GetName());
    label->SetTextStyle(EditorStyle::SmallText());
    label->SetDesiredSize(Vector2(100.0f, 20.0f));
}

void SceneTreeNode::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    layout->Arrange(rect);
}

void SceneTreeNode::OnUpdate(float deltaTime)
{
    Render::TextStyle style = EditorStyle::SmallText();

    if (IsHovered())
    {
        style.color = Color::FromRGB(0xA0, 0xA0, 0xA0);
    }

    label->SetTextStyle(style);
}

} // namespace URay
