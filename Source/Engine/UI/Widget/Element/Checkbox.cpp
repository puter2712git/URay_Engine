#include "Checkbox.h"
#include <Core/Math/Math.h>

#include <Core/Math/Color.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay
{

Checkbox::Checkbox()
{
    URAY_DECLARE_RAY(ValueChanged, bool);
}

void Checkbox::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    const float side = Math::Max(0.0f, Math::Min(rect.size.x, rect.size.y));

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
        Emit("ValueChanged", checked);
    }

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

void Checkbox::OnPaint(Render::UIDrawContext& context)
{
    Color background = normalColor;

    if (HasPointerCapture())
        background = pressedColor;
    else if (IsHovered())
        background = hoverColor;

    context.AddFilledRect(indicatorRect, background);
    context.AddOutlineRect(indicatorRect, 1.0f, outlineColor);

    if (!checked)
        return;

    constexpr float inset = 4.0f;

    Rect markRect = {};
    markRect.position = indicatorRect.position + Vector2(inset, inset);
    markRect.size = Vector2(
        Math::Max(0.0f, indicatorRect.size.x - inset * 2.0f),
        Math::Max(0.0f, indicatorRect.size.y - inset * 2.0f));

    context.AddFilledRect(markRect, checkmarkColor);
}

} // namespace URay
