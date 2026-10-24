#pragma once

#include <Engine/UI/Widget.h>

#include <Core/Ray/EventRay.h>

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

    EventRay<bool>& GetValueChangeRay() { return valueChangeRay; }

protected:
    Vector2 OnMeasure() override { return Vector2(18.0f, 18.0f); }

    void OnPaint(Render::UIDrawContext& context) override;

private:
    Rect indicatorRect = {};
    bool checked = false;

    EventRay<bool> valueChangeRay;
};

} // namespace URay
