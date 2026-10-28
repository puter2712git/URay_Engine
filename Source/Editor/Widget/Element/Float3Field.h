#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Core/Math/Vector3.h>
#include <Core/Type/Ray/Ray.h>
#include <Core/Type/Ray/RayEmitter.h>

namespace URay
{

class Label;
class NumberField;

class Float3Field final : public Widget, public RayEmitter
{
public:
    Float3Field();

public:
    void SetValue(const Vector3& value);

    void Arrange(const Rect& rect) override;

protected:
    Vector2 OnMeasure() override;

private:
    void OnXChanged(float component);
    void OnYChanged(float component);
    void OnZChanged(float component);

    void ArrangeComponent(
        const Rect& rect,
        Label& label,
        NumberField& field) const;

public:
    Ray<Vector3> onValueChanged;

private:
    Vector3 value = Vector3::Zero;

    Label* xLabel = nullptr;
    Label* yLabel = nullptr;
    Label* zLabel = nullptr;

    NumberField* xField = nullptr;
    NumberField* yField = nullptr;
    NumberField* zField = nullptr;
};

} // namespace URay
