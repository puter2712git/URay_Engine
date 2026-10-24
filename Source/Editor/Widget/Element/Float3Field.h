#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Core/Math/Vector3.h>
#include <Core/Ray/EventRay.h>

namespace URay
{

class Label;
class NumberField;

class Float3Field final : public Widget
{
public:
    Float3Field();

public:
    void SetValue(const Vector3& value);

    EventRay<Vector3>& GetValueChangedRay() { return onValueChanged; }

    void Arrange(const Rect& rect) override;

protected:
    Vector2 OnMeasure() override;

private:
    void ArrangeComponent(
        const Rect& rect,
        Label& label,
        NumberField& field) const;

private:
    Vector3 value = Vector3::Zero;

    Label* xLabel = nullptr;
    Label* yLabel = nullptr;
    Label* zLabel = nullptr;

    NumberField* xField = nullptr;
    NumberField* yField = nullptr;
    NumberField* zField = nullptr;

    EventRay<Vector3> onValueChanged;
};

} // namespace URay
