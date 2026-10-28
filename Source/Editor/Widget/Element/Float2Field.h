#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Core/Math/Vector2.h>
#include <Core/Type/Ray/RayEmitter.h>

namespace URay
{

class Label;
class NumberField;

class Float2Field final : public Widget, public RayEmitter
{
public:
    Float2Field();

    void SetValue(const Vector2& value);

    void Arrange(const Rect& rect) override;

protected:
    Vector2 OnMeasure() override;

private:
    void ArrangeComponent(const Rect& rect, Label& label, NumberField& field) const;
    void OnXChanged(float component);
    void OnYChanged(float component);

private:
    Vector2 value = Vector2::Zero;

    Label* xLabel = nullptr;
    Label* yLabel = nullptr;

    NumberField* xField = nullptr;
    NumberField* yField = nullptr;

};

} // namespace URay
