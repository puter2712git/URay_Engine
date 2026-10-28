#include "Float3Field.h"

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Core/Math/Math.h>

#include <Engine/UI/Widget/Element/Label.h>
#include <Editor/Widget/Element/NumberField.h>

#include <algorithm>

namespace URay
{

namespace
{

constexpr float ComponentGap = 4.0f;
constexpr float LabelFieldGap = 4.0f;

}

Float3Field::Float3Field()
{
    URAY_DECLARE_RAY(ValueChanged, Vector3);

    xLabel = CreateEditorLabel(*this, "X");
    xField = AddChild<NumberField>();

    yLabel = CreateEditorLabel(*this, "Y");
    yField = AddChild<NumberField>();

    zLabel = CreateEditorLabel(*this, "Z");
    zField = AddChild<NumberField>();

    xField->Register("ValueChanged", this, &Float3Field::OnXChanged);
    yField->Register("ValueChanged", this, &Float3Field::OnYChanged);
    zField->Register("ValueChanged", this, &Float3Field::OnZChanged);
}

void Float3Field::OnXChanged(float component)
{
    value.x = component;
    Emit("ValueChanged", value);
}

void Float3Field::OnYChanged(float component)
{
    value.y = component;
    Emit("ValueChanged", value);
}

void Float3Field::OnZChanged(float component)
{
    value.z = component;
    Emit("ValueChanged", value);
}

void Float3Field::SetValue(const Vector3& value)
{
    this->value = value;

    xField->SetValue(value.x);
    yField->SetValue(value.y);
    zField->SetValue(value.z);
}

void Float3Field::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    const float componentWidth = Math::Max(
        0.0f,
        (rect.size.x - ComponentGap * 2.0f) / 3.0f);

    Rect componentRect;
    componentRect.position = rect.position;
    componentRect.size = Vector2(componentWidth, rect.size.y);
    ArrangeComponent(componentRect, *xLabel, *xField);

    componentRect.position.x += componentWidth + ComponentGap;
    ArrangeComponent(componentRect, *yLabel, *yField);

    componentRect.position.x += componentWidth + ComponentGap;
    ArrangeComponent(componentRect, *zLabel, *zField);
}

Vector2 Float3Field::OnMeasure()
{
    const Vector2 xLabelSize = xLabel->GetDesiredSize();
    const Vector2 xFieldSize = xField->GetDesiredSize();
    const Vector2 yLabelSize = yLabel->GetDesiredSize();
    const Vector2 yFieldSize = yField->GetDesiredSize();
    const Vector2 zLabelSize = zLabel->GetDesiredSize();
    const Vector2 zFieldSize = zField->GetDesiredSize();

    const float xWidth = xLabelSize.x + LabelFieldGap + xFieldSize.x;
    const float yWidth = yLabelSize.x + LabelFieldGap + yFieldSize.x;
    const float zWidth = zLabelSize.x + LabelFieldGap + zFieldSize.x;

    const float height = Math::Max({
        xLabelSize.y,
        xFieldSize.y,
        yLabelSize.y,
        yFieldSize.y,
        zLabelSize.y,
        zFieldSize.y,
    });

    return Vector2(
        xWidth + yWidth + zWidth + ComponentGap * 2.0f,
        height);
}

void Float3Field::ArrangeComponent(
    const Rect& rect,
    Label& label,
    NumberField& field) const
{
    const Vector2 labelSize = label.GetDesiredSize();
    const float labelWidth = labelSize.x;

    Rect labelRect;
    labelRect.position = Vector2(
        rect.position.x,
        rect.position.y + (rect.size.y - labelSize.y) * 0.5f);
    labelRect.size = labelSize;
    label.Arrange(labelRect);

    Rect fieldRect;
    fieldRect.position = Vector2(rect.position.x + labelWidth + LabelFieldGap, rect.position.y);
    fieldRect.size = Vector2(
        Math::Max(0.0f, rect.size.x - labelWidth - LabelFieldGap),
        rect.size.y);
    field.Arrange(fieldRect);
}

} // namespace URay
