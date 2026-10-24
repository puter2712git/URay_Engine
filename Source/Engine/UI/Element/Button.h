#pragma once

#include <Engine/UI/Widget.h>

#include <Core/Math/Color.h>
#include <Core/Ray/EventRay.h>

#include <memory>

namespace URay
{

enum class ButtonState
{
    Normal,
    Hovering,
    Pressed
};

class Button final : public Widget
{
    URAY_TYPE(Button, Widget)

public:
    explicit Button(std::unique_ptr<Widget> content);
    ~Button() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    ButtonState GetState() const;

    Widget& GetContent() const { return *content; }

    void SetNormalColor(const Color& color) { normalColor = color; }
    void SetHoverColor(const Color& color) { hoverColor = color; }
    void SetPressedColor(const Color& color) { pressedColor = color; }

    EventRay<>& GetOnClickRay() { return onClicked; }

protected:
    Vector2 OnMeasure() override;

    void OnPaint(Render::UIDrawContext& context) override;

private:
    Widget* content = nullptr;

    Color normalColor = {};
    Color hoverColor = {};
    Color pressedColor = {};

    EventRay<> onClicked;
};

} // namespace URay
