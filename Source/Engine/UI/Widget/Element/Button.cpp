#include "Button.h"

#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay
{

Button::Button(std::unique_ptr<Widget> content)
{
    URAY_DECLARE_RAY(Clicked);
    this->content = content.get();
    AddChild(std::move(content));
}

Button::~Button() = default;

void Button::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    content->Arrange(rect);
}

EventReply Button::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    return EventReply{
        .handled = true,
        .requestFocus = true,
        .capturePointer = true,
    };
}

EventReply Button::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    if (IsHovered())
    {
        onClicked.Emit();
        Emit("Clicked");
    }

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

ButtonState Button::GetState() const
{
    if (HasPointerCapture())
        return ButtonState::Pressed;

    if (IsHovered())
        return ButtonState::Hovering;

    return ButtonState::Normal;
}

Vector2 Button::OnMeasure()
{
    return content->GetDesiredSize();
}

void Button::OnPaint(Render::UIDrawContext& context)
{
    Color color = normalColor;

    switch (GetState())
    {
    case ButtonState::Normal:
        color = normalColor;
        break;
    case ButtonState::Hovering:
        color = hoverColor;
        break;
    case ButtonState::Pressed:
        color = pressedColor;
        break;
    default:
        break;
    }

    context.AddFilledRect(rect, color);
}

} // namespace URay
