#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Core/Math/Color.h>
#include <Core/Ray/EventRay.h>

namespace URay
{

class Label;
class NumberField;

class ColorField final : public Widget
{
public:
    ColorField();

public:
    void SetValue(const Color& value);

    EventRay<Color>& GetValueChangedRay() { return onValueChanged; }

    void Arrange(const Rect& rect) override;

protected:
    Vector2 OnMeasure() override;
    void OnPaint(Render::UIDrawContext& context) override;

private:
    void ArrangeComponent(
        const Rect& rect,
        Label& label,
        NumberField& field) const;

private:
    Color value = Color::White;
    Rect previewRect;

    Label* rLabel = nullptr;
    Label* gLabel = nullptr;
    Label* bLabel = nullptr;
    Label* aLabel = nullptr;

    NumberField* rField = nullptr;
    NumberField* gField = nullptr;
    NumberField* bField = nullptr;
    NumberField* aField = nullptr;

    EventRay<Color> onValueChanged;
};

} // namespace URay
