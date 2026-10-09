#include "PropertyRow.h"

#include <Editor/Widget/Element/Label.h>

namespace URay
{

PropertyRow::PropertyRow(const std::string& name)
{
    nameLabel = AddChild<Label>(name);
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
        std::max(0.0f, rect.size.x - nameWidth - gap),
        rect.size.y);

    nameLabel->Arrange(nameRect);

    if (valueWidget)
    {
        valueWidget->Arrange(valueRect);
    }
}

} // namespace URay
