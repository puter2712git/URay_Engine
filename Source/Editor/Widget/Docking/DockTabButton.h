#pragma once

#include <Core/Math/Color.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Engine/UI/Widget/Widget.h>

namespace URay
{

class Label;

class DockTabButton final : public Widget, public RayEmitter
{
    URAY_CLASS(DockTabButton, Widget)

public:
    explicit DockTabButton(StringView title);

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    void SetNormalColor(const Color& color) { normalColor = color; }
    void SetHoverColor(const Color& color) { hoverColor = color; }
    void SetPressedColor(const Color& color) { pressedColor = color; }

protected:
    Vector2 OnMeasure() override;

    void OnPaint(Render::UIDrawContext& context) override;

private:
    Label* label = nullptr;

    bool isPointerDown = false;
    bool isDragging = false;
    Vector2 pressPosition = Vector2::Zero;

    Color normalColor = {};
    Color hoverColor = {};
    Color pressedColor = {};
};

} // namespace URay
