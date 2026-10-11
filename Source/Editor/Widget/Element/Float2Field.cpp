#include "Float2Field.h"

#include <Editor/Widget/Element/Label.h>
#include <Editor/Widget/Element/NumberField.h>

#include <algorithm>

namespace URay
{

namespace
{

constexpr float ComponentGap = 4.0f;
constexpr float LabelFieldGap = 4.0f;

}

Float2Field::Float2Field()
{
    xLabel = AddChild<Label>("X");
    xField = AddChild<NumberField>();
    yLabel = AddChild<Label>("Y");
    yField = AddChild<NumberField>();

    xField->GetValueChangedRay().Register(
        this,
        [this](float component)
        {
            value.x = component;
            valueChangedRay.Emit(value);
        });

    yField->GetValueChangedRay().Register(
        this,
        [this](float component)
        {
            value.y = component;
            valueChangedRay.Emit(value);
        });
}

void Float2Field::SetValue(const Vector2& value)
{
    this->value = value;
    xField->SetValue(value.x);
    yField->SetValue(value.y);
}

void Float2Field::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    const float componentWidth = std::max(0.0f, (rect.size.x - ComponentGap) / 2.0f);

    Rect componentRect{ .position = rect.position, .size = Vector2(componentWidth, rect.size.y) };
    ArrangeComponent(componentRect, *xLabel, *xField);

    componentRect.position.x += componentWidth + ComponentGap;
    ArrangeComponent(componentRect, *yLabel, *yField);
}

Vector2 Float2Field::OnMeasure()
{
    const Vector2 xLabelSize = xLabel->GetDesiredSize();
    const Vector2 xFieldSize = xField->GetDesiredSize();
    const Vector2 yLabelSize = yLabel->GetDesiredSize();
    const Vector2 yFieldSize = yField->GetDesiredSize();

    return Vector2(
        xLabelSize.x + xFieldSize.x + yLabelSize.x + yFieldSize.x + LabelFieldGap * 2.0f + ComponentGap,
        std::max({ xLabelSize.y, xFieldSize.y, yLabelSize.y, yFieldSize.y }));
}

void Float2Field::ArrangeComponent(const Rect& rect, Label& label, NumberField& field) const
{
    const Vector2 labelSize = label.GetDesiredSize();

    label.Arrange({
        .position = Vector2(rect.position.x, rect.position.y + (rect.size.y - labelSize.y) * 0.5f),
        .size = labelSize,
    });

    field.Arrange({
        .position = Vector2(rect.position.x + labelSize.x + LabelFieldGap, rect.position.y),
        .size = Vector2(std::max(0.0f, rect.size.x - labelSize.x - LabelFieldGap), rect.size.y),
    });
}

} // namespace URay
