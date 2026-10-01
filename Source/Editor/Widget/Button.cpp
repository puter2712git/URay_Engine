#include "Button.h"

#include "Editor/Widget/Label.h"

#include "Engine/Asset/EngineAsset.h"

#include <memory>

namespace URay
{

Button::Button(std::string_view text)
{
    auto buttonLabel = std::make_unique<Label>(text);
    buttonLabel->SetTextStyle({ .fontHandle = EngineAsset::EditorFont,
                                .pixelHeight = 18,
                                .color = Color::Black });
    label = buttonLabel.get();
    children.push_back(std::move(buttonLabel));
}

Button::~Button() = default;

void Button::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    label->Arrange(rect);
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
        onClickRay.Emit();
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
