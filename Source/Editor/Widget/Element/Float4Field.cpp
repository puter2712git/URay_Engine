#include "Float4Field.h"

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Core/Math/Math.h>

#include <Engine/UI/Element/Label.h>
#include <Editor/Widget/Element/NumberField.h>

#include <algorithm>

namespace URay
{

namespace
{

constexpr float ComponentGap = 4.0f;
constexpr float LabelFieldGap = 4.0f;

}

Float4Field::Float4Field()
{
    xLabel = CreateEditorLabel(*this, "X");
    xField = AddChild<NumberField>();
    yLabel = CreateEditorLabel(*this, "Y");
    yField = AddChild<NumberField>();
    zLabel = CreateEditorLabel(*this, "Z");
    zField = AddChild<NumberField>();
    wLabel = CreateEditorLabel(*this, "W");
    wField = AddChild<NumberField>();

    xField->GetValueChangedRay().Register(this, [this](float component) { value.x = component; onValueChanged.Emit(value); });
    yField->GetValueChangedRay().Register(this, [this](float component) { value.y = component; onValueChanged.Emit(value); });
    zField->GetValueChangedRay().Register(this, [this](float component) { value.z = component; onValueChanged.Emit(value); });
    wField->GetValueChangedRay().Register(this, [this](float component) { value.w = component; onValueChanged.Emit(value); });
}

void Float4Field::SetValue(const Vector4& value)
{
    this->value = value;
    xField->SetValue(value.x);
    yField->SetValue(value.y);
    zField->SetValue(value.z);
    wField->SetValue(value.w);
}

void Float4Field::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    const float componentWidth = Math::Max(0.0f, (rect.size.x - ComponentGap * 3.0f) / 4.0f);

    Rect componentRect{ .position = rect.position, .size = Vector2(componentWidth, rect.size.y) };
    ArrangeComponent(componentRect, *xLabel, *xField);
    componentRect.position.x += componentWidth + ComponentGap;
    ArrangeComponent(componentRect, *yLabel, *yField);
    componentRect.position.x += componentWidth + ComponentGap;
    ArrangeComponent(componentRect, *zLabel, *zField);
    componentRect.position.x += componentWidth + ComponentGap;
    ArrangeComponent(componentRect, *wLabel, *wField);
}

Vector2 Float4Field::OnMeasure()
{
    const Vector2 xLabelSize = xLabel->GetDesiredSize();
    const Vector2 xFieldSize = xField->GetDesiredSize();
    const Vector2 yLabelSize = yLabel->GetDesiredSize();
    const Vector2 yFieldSize = yField->GetDesiredSize();
    const Vector2 zLabelSize = zLabel->GetDesiredSize();
    const Vector2 zFieldSize = zField->GetDesiredSize();
    const Vector2 wLabelSize = wLabel->GetDesiredSize();
    const Vector2 wFieldSize = wField->GetDesiredSize();

    return Vector2(
        xLabelSize.x + xFieldSize.x + yLabelSize.x + yFieldSize.x +
            zLabelSize.x + zFieldSize.x + wLabelSize.x + wFieldSize.x +
            LabelFieldGap * 4.0f + ComponentGap * 3.0f,
        Math::Max({
            xLabelSize.y, xFieldSize.y, yLabelSize.y, yFieldSize.y,
            zLabelSize.y, zFieldSize.y, wLabelSize.y, wFieldSize.y,
        }));
}

void Float4Field::ArrangeComponent(const Rect& rect, Label& label, NumberField& field) const
{
    const Vector2 labelSize = label.GetDesiredSize();

    label.Arrange({
        .position = Vector2(rect.position.x, rect.position.y + (rect.size.y - labelSize.y) * 0.5f),
        .size = labelSize,
    });

    field.Arrange({
        .position = Vector2(rect.position.x + labelSize.x + LabelFieldGap, rect.position.y),
        .size = Vector2(Math::Max(0.0f, rect.size.x - labelSize.x - LabelFieldGap), rect.size.y),
    });
}

} // namespace URay
