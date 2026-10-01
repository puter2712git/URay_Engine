#pragma once

#include "Editor/Widget/Widget.h"

#include "Engine/Ray/EventRay.h"

#include "Core/Math/Color.h"

#include <string_view>

namespace URay
{

class Label;

enum class ButtonState
{
    Normal,
    Hovering,
    Pressed
};

class Button final : public Widget
{
public:
    Button(std::string_view text = {});
    ~Button() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    ButtonState GetState() const;

    EventRay<>& GetOnClickRay() { return onClickRay; }

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    Label* label = nullptr;

    Color normalColor = Color::White;
    Color hoverColor = Color::Blue;
    Color pressedColor = Color::Green;

    EventRay<> onClickRay;
};

} // namespace URay
