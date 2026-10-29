#include "DockTabButton.h"

#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay
{

DockTabButton::DockTabButton(StringView title)
{
    URAY_DECLARE_RAY(Clicked);
    URAY_DECLARE_RAY(DragMoved, Vector2);
    URAY_DECLARE_RAY(DragReleased, Vector2);

    label = AddChild<Label>(title);
    label->SetTextStyle(EditorStyle::SmallText());
    label->SetHorizontalAlignment(HorizontalTextAlignment::Center);
    label->SetVerticalAlignment(VerticalTextAlignment::Center);
}

void DockTabButton::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);
    label->Arrange(rect);
}

EventReply DockTabButton::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    isPointerDown = true;
    isDragging = false;
    pressPosition = event.position;

    return EventReply{
        .handled = true,
        .requestFocus = true,
        .capturePointer = true
    };
}

EventReply DockTabButton::OnPointerMove(const PointerEvent& event)
{
    if (!isPointerDown)
        return {};

    constexpr float dragThreshold = 4.0f;

    const float dx = event.position.x - pressPosition.x;
    const float dy = event.position.y - pressPosition.y;

    if (!isDragging && dx * dx + dy * dy >= dragThreshold * dragThreshold)
    {
        isDragging = true;
    }

    if (isDragging)
        Emit("DragMoved", event.position);

    return EventReply{ .handled = true };
}

EventReply DockTabButton::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || !isPointerDown)
        return {};

    const bool wasDragging = isDragging;

    isPointerDown = false;
    isDragging = false;

    if (wasDragging)
    {
        Emit("DragReleased", event.position);
    }
    else if (IsHovered())
    {
        Emit("Clicked");
    }

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

Vector2 DockTabButton::OnMeasure()
{
    return label->GetDesiredSize();
}

void DockTabButton::OnPaint(Render::UIDrawContext& context)
{
    Color color = normalColor;

    if (HasPointerCapture())
        color = pressedColor;
    else if (IsHovered())
        color = hoverColor;

    context.AddFilledRect(rect, color);
}

} // namespace URay
