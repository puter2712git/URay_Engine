#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Core/Math/Vector4.h>
#include <Core/Ray/EventRay.h>

namespace URay
{

class Label;
class NumberField;

class Float4Field final : public Widget
{
public:
    Float4Field();

    void SetValue(const Vector4& value);

    void Arrange(const Rect& rect) override;

protected:
    Vector2 OnMeasure() override;

private:
    void ArrangeComponent(const Rect& rect, Label& label, NumberField& field) const;

public:
    EventRay<Vector4> onValueChanged;

private:
    Vector4 value = {};

    Label* xLabel = nullptr;
    Label* yLabel = nullptr;
    Label* zLabel = nullptr;
    Label* wLabel = nullptr;

    NumberField* xField = nullptr;
    NumberField* yField = nullptr;
    NumberField* zField = nullptr;
    NumberField* wField = nullptr;

};

} // namespace URay
