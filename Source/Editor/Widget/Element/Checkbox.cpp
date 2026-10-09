#include "Checkbox.h"

#include <Editor/Widget/Style/EditorStyles.h>

#include <Core/Math/Color.h>

namespace URay
{

Checkbox::Checkbox()
{
    SetDesiredSize(Vector2(18.0f, 18.0f));
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

    context.AddFilledRect(rect, background);
    context.AddOutlineRect(rect, 1.0f, EditorStyle::Border);

    if (!checked)
        return;

    constexpr float inset = 4.0f;

    Rect markRect = {};
    markRect.position = rect.position + Vector2(inset, inset);
    markRect.size = Vector2(
        std::max(0.0f, rect.size.x - inset * 2.0f),
        std::max(0.0f, rect.size.y - inset * 2.0f));

    context.AddFilledRect(markRect, EditorStyle::TextPrimary);
}

} // namespace URay
