#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Core/Math/Color.h>
#include <Core/Type/Ray.h>

namespace URay
{

class Checkbox : public Widget
{
public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    bool IsChecked() const { return checked; }
    void SetChecked(bool value) { checked = value; }

    void SetNormalColor(const Color& color) { normalColor = color; }
    void SetHoverColor(const Color& color) { hoverColor = color; }
    void SetPressedColor(const Color& color) { pressedColor = color; }
    void SetOutlineColor(const Color& color) { outlineColor = color; }
    void SetCheckmarkColor(const Color& color) { checkmarkColor = color; }

protected:
    Vector2 OnMeasure() override { return Vector2(18.0f, 18.0f); }

    void OnPaint(Render::UIDrawContext& context) override;

public:
    Ray<bool> onValueChanged;

private:
    Rect indicatorRect = {};
    bool checked = false;

    Color normalColor = {};
    Color hoverColor = {};
    Color pressedColor = {};
    Color outlineColor = {};
    Color checkmarkColor = {};

};

} // namespace URay
