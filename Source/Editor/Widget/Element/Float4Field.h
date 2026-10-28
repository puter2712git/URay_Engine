#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Core/Math/Vector4.h>
#include <Core/Type/Ray/Ray.h>
#include <Core/Type/Ray/RayEmitter.h>

namespace URay
{

class Label;
class NumberField;

class Float4Field final : public Widget, public RayEmitter
{
public:
    Float4Field();

    void SetValue(const Vector4& value);

    void Arrange(const Rect& rect) override;

protected:
    Vector2 OnMeasure() override;

private:
    void ArrangeComponent(const Rect& rect, Label& label, NumberField& field) const;
    void OnXChanged(float component);
    void OnYChanged(float component);
    void OnZChanged(float component);
    void OnWChanged(float component);

public:
    Ray<Vector4> onValueChanged;

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
