#include "HorizontalLayout.h"

#include <algorithm>

namespace URay
{

void HorizontalLayout::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    Rect contentRect;
    contentRect.position = Vector2(rect.position.x + padding.left, rect.position.y + padding.top);
    contentRect.size = contentRect.size = {
        std::max(0.0f, rect.size.x - padding.left - padding.right),
        std::max(0.0f, rect.size.y - padding.top - padding.bottom),
    };

    float offsetX = CalculateStartOffsetX(contentRect);

    for (const auto& child : children)
    {
        const float childWidth = GetChildLayoutWidth(*child);
        const float childHeight = CalculateChildHeight(*child, contentRect);

        Rect childRect;
        childRect.position = Vector2(offsetX, CalculateChildY(childHeight, contentRect));
        childRect.size = Vector2(childWidth, childHeight);

        child->Arrange(childRect);

        offsetX += childWidth + gap;
    }
}

Vector2 HorizontalLayout::OnMeasure()
{
    float width = padding.left + padding.right;
    float height = 0.0f;

    for (size_t index = 0; index < children.size(); ++index)
    {
        Widget& child = *children[index];

        width += GetChildLayoutWidth(child);
        height = std::max(height, GetChildLayoutHeight(child));

        if (index + 1 < children.size())
            width += gap;
    }

    height += padding.top + padding.bottom;
    return Vector2(width, height);
}

float HorizontalLayout::CalculateStartOffsetX(const Rect& contentRect) const
{
    switch (mainAxisAlignment)
    {
    case MainAxisAlignment::Start:
        return contentRect.position.x;

    case MainAxisAlignment::Center:
    {
        float totalWidth = CalculateTotalWidth();
        return contentRect.position.x + contentRect.size.x * 0.5f - totalWidth * 0.5f;
    }

    case MainAxisAlignment::End:
    {
        float totalWidth = CalculateTotalWidth();
        return contentRect.position.x + contentRect.size.x - totalWidth;
    }

    default:
        return 0.0f;
    }
}

float HorizontalLayout::CalculateTotalWidth() const
{
    float totalWidth = 0.0f;

    for (const auto& child : children)
    {
        totalWidth += GetChildLayoutWidth(*child);
    }

    int32 childCount = static_cast<int32>(children.size());
    int32 gapCount = std::max(0, childCount - 1);

    totalWidth += gap * gapCount;

    return totalWidth;
}

float HorizontalLayout::CalculateChildY(float childHeight, const Rect& contentRect) const
{
    switch (crossAxisAlignment)
    {
    case CrossAxisAlignment::Start:
        return contentRect.position.y;

    case CrossAxisAlignment::Center:
        return contentRect.position.y + contentRect.size.y * 0.5f - childHeight * 0.5f;

    case CrossAxisAlignment::End:
        return contentRect.position.y + contentRect.size.y - childHeight;

    case CrossAxisAlignment::Stretch:
        return contentRect.position.y;

    default:
        return contentRect.position.y;
    }
}

float HorizontalLayout::CalculateChildHeight(const Widget& child, const Rect& contentRect) const
{
    switch (crossAxisAlignment)
    {
    case CrossAxisAlignment::Start:
    case CrossAxisAlignment::Center:
    case CrossAxisAlignment::End:
        return GetChildLayoutHeight(child);

    case CrossAxisAlignment::Stretch:
        return contentRect.size.y;

    default:
        return GetChildLayoutHeight(child);
    }
}

float HorizontalLayout::GetChildLayoutWidth(const Widget& child) const
{
    return std::max(0.0f, child.GetPreferredWidth().value_or(child.GetDesiredSize().x));
}

float HorizontalLayout::GetChildLayoutHeight(const Widget& child) const
{
    return std::max(0.0f, child.GetPreferredHeight().value_or(child.GetDesiredSize().y));
}

} // namespace URay
