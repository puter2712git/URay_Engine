#include "ColorField.h"

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Core/Math/Math.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Editor/Widget/Element/NumberField.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <algorithm>

namespace URay
{

namespace
{

constexpr float PreviewSize = 18.0f;
constexpr float PreviewGap = 6.0f;
constexpr float ComponentGap = 4.0f;
constexpr float LabelFieldGap = 4.0f;

Render::TextStyle MakeComponentLabelStyle(const Color& color)
{
    Render::TextStyle style = EditorStyle::SmallText();
    style.color = color;
    return style;
}

} // namespace

ColorField::ColorField()
{
    rLabel = CreateEditorLabel(*this, "R");
    rLabel->SetTextStyle(MakeComponentLabelStyle(Color::Red));
    rField = AddChild<NumberField>();

    gLabel = CreateEditorLabel(*this, "G");
    gLabel->SetTextStyle(MakeComponentLabelStyle(Color::Green));
    gField = AddChild<NumberField>();

    bLabel = CreateEditorLabel(*this, "B");
    bLabel->SetTextStyle(MakeComponentLabelStyle(Color::Blue));
    bField = AddChild<NumberField>();

    aLabel = CreateEditorLabel(*this, "A");
    aLabel->SetTextStyle(MakeComponentLabelStyle(EditorStyle::TextPrimary));
    aField = AddChild<NumberField>();

    rField->GetValueChangedRay().Register(
        this,
        [this](float component)
        {
            value.r = component;
            onValueChanged.Emit(value);
        });

    gField->GetValueChangedRay().Register(
        this,
        [this](float component)
        {
            value.g = component;
            onValueChanged.Emit(value);
        });

    bField->GetValueChangedRay().Register(
        this,
        [this](float component)
        {
            value.b = component;
            onValueChanged.Emit(value);
        });

    aField->GetValueChangedRay().Register(
        this,
        [this](float component)
        {
            value.a = component;
            onValueChanged.Emit(value);
        });
}

void ColorField::SetValue(const Color& value)
{
    this->value = value;

    rField->SetValue(value.r);
    gField->SetValue(value.g);
    bField->SetValue(value.b);
    aField->SetValue(value.a);
}

void ColorField::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    const float previewSide = Math::Min(PreviewSize, Math::Max(0.0f, rect.size.y));
    previewRect.position = Vector2(
        rect.position.x,
        rect.position.y + (rect.size.y - previewSide) * 0.5f);
    previewRect.size = Vector2(previewSide, previewSide);

    const float fieldsStartX = rect.position.x + previewSide + PreviewGap;
    const float availableWidth = Math::Max(0.0f, rect.size.x - previewSide - PreviewGap);
    const float componentWidth = Math::Max(
        0.0f,
        (availableWidth - ComponentGap * 3.0f) / 4.0f);

    Rect componentRect;
    componentRect.position = Vector2(fieldsStartX, rect.position.y);
    componentRect.size = Vector2(componentWidth, rect.size.y);
    ArrangeComponent(componentRect, *rLabel, *rField);

    componentRect.position.x += componentWidth + ComponentGap;
    ArrangeComponent(componentRect, *gLabel, *gField);

    componentRect.position.x += componentWidth + ComponentGap;
    ArrangeComponent(componentRect, *bLabel, *bField);

    componentRect.position.x += componentWidth + ComponentGap;
    ArrangeComponent(componentRect, *aLabel, *aField);
}

Vector2 ColorField::OnMeasure()
{
    const Vector2 rLabelSize = rLabel->GetDesiredSize();
    const Vector2 rFieldSize = rField->GetDesiredSize();
    const Vector2 gLabelSize = gLabel->GetDesiredSize();
    const Vector2 gFieldSize = gField->GetDesiredSize();
    const Vector2 bLabelSize = bLabel->GetDesiredSize();
    const Vector2 bFieldSize = bField->GetDesiredSize();
    const Vector2 aLabelSize = aLabel->GetDesiredSize();
    const Vector2 aFieldSize = aField->GetDesiredSize();

    const float rWidth = rLabelSize.x + LabelFieldGap + rFieldSize.x;
    const float gWidth = gLabelSize.x + LabelFieldGap + gFieldSize.x;
    const float bWidth = bLabelSize.x + LabelFieldGap + bFieldSize.x;
    const float aWidth = aLabelSize.x + LabelFieldGap + aFieldSize.x;

    const float height = Math::Max({
        PreviewSize,
        rLabelSize.y,
        rFieldSize.y,
        gLabelSize.y,
        gFieldSize.y,
        bLabelSize.y,
        bFieldSize.y,
        aLabelSize.y,
        aFieldSize.y,
    });

    return Vector2(
        PreviewSize + PreviewGap +
            rWidth + gWidth + bWidth + aWidth + ComponentGap * 3.0f,
        height);
}

void ColorField::OnPaint(Render::UIDrawContext& context)
{
    context.AddFilledRect(previewRect, value);
    context.AddOutlineRect(previewRect, 1.0f, EditorStyle::Border);
}

void ColorField::ArrangeComponent(
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
