#include "Checkbox.h"

#include <Editor/Widget/Style/EditorStyles.h>

#include <Core/Math/Color.h>

namespace URay
{

Checkbox::Checkbox()
{
    SetDesiredSize(Vector2(18.0f, 18.0f));
}

void Checkbox::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    const float side = std::max(0.0f, std::min(rect.size.x, rect.size.y));

    indicatorRect.position = Vector2(
        rect.position.x,
        rect.position.y + (rect.size.y - side) * 0.5f);
    indicatorRect.size = Vector2(side, side);
}

EventReply Checkbox::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    return EventReply{
        .handled = true,
        .requestFocus = true,
        .capturePointer = true
    };
}

EventReply Checkbox::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    if (IsHovered())
    {
        checked = !checked;
        valueChangeRay.Emit(checked);
    }

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

void Checkbox::OnPaint(Render::UIDrawContext& context)
{
    Color background = EditorStyle::ControlNormal;

    if (HasPointerCapture())
        background = EditorStyle::ControlPressed;
    else if (IsHovered())
        background = EditorStyle::ControlHovered;

    context.AddFilledRect(indicatorRect, background);
    context.AddOutlineRect(indicatorRect, 1.0f, EditorStyle::Border);

    if (!checked)
        return;

    constexpr float inset = 4.0f;

    Rect markRect = {};
    markRect.position = indicatorRect.position + Vector2(inset, inset);
    markRect.size = Vector2(
        std::max(0.0f, indicatorRect.size.x - inset * 2.0f),
        std::max(0.0f, indicatorRect.size.y - inset * 2.0f));

    context.AddFilledRect(markRect, EditorStyle::TextPrimary);
}

} // namespace URay
