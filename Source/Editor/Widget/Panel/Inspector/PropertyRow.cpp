#include "PropertyRow.h"

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Core/Math/Math.h>

#include <Engine/UI/Widget/Element/Label.h>

namespace URay
{

PropertyRow::PropertyRow(const std::string& name)
{
    nameLabel = CreateEditorLabel(*this, name);
}

void PropertyRow::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    constexpr float nameWidth = 250.0f;
    constexpr float gap = 8.0f;

    Rect nameRect;
    nameRect.position = rect.position;
    nameRect.size = Vector2(nameWidth, rect.size.y);

    Rect valueRect;
    valueRect.position = Vector2(
        rect.position.x + nameWidth + gap,
        rect.position.y);
    valueRect.size = Vector2(
        Math::Max(0.0f, rect.size.x - nameWidth - gap),
        rect.size.y);

    nameLabel->Arrange(nameRect);

    if (valueWidget)
    {
        valueWidget->Arrange(valueRect);
    }
}

Vector2 PropertyRow::OnMeasure()
{
    constexpr float nameWidth = 250.0f;
    constexpr float gap = 8.0f;

    const Vector2 nameSize = nameLabel->GetDesiredSize();
    const Vector2 valueSize = valueWidget
                                  ? valueWidget->GetDesiredSize()
                                  : Vector2::Zero;

    return Vector2(
        nameWidth + gap + valueSize.x,
        Math::Max(nameSize.y, valueSize.y));
}

} // namespace URay
