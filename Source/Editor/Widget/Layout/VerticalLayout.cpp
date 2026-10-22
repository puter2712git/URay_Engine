#include "VerticalLayout.h"
#include <Core/Math/Math.h>

#include <algorithm>

namespace URay
{

void VerticalLayout::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    Rect contentRect;
    contentRect.position = Vector2(rect.position.x + padding.left, rect.position.y + padding.top);
    contentRect.size = {
        Math::Max(0.0f, rect.size.x - padding.left - padding.right),
        Math::Max(0.0f, rect.size.y - padding.top - padding.bottom),
    };

    float offsetY = CalculateStartOffsetY(contentRect);

    for (const auto& child : children)
    {
        const float childWidth = CalculateChildWidth(*child, contentRect);
        const float childHeight = GetChildLayoutHeight(*child);

        Rect childRect;
        childRect.position = Vector2(CalculateChildX(childWidth, contentRect), offsetY);
        childRect.size = Vector2(childWidth, childHeight);

        child->Arrange(childRect);

        offsetY += childHeight + gap;
    }
}

Vector2 VerticalLayout::OnMeasure()
{
    float width = 0.0f;
    float height = padding.top + padding.bottom;

    for (size_t index = 0; index < children.size(); ++index)
    {
        Widget& child = *children[index];

        width = Math::Max(width, GetChildLayoutWidth(child));
        height += GetChildLayoutHeight(child);

        if (index + 1 < children.size())
            height += gap;
    }

    width += padding.left + padding.right;
    return Vector2(width, height);
}

float VerticalLayout::CalculateStartOffsetY(const Rect& contentRect) const
{
    switch (mainAxisAlignment)
    {
    case MainAxisAlignment::Start:
        return contentRect.position.y;

    case MainAxisAlignment::Center:
    {
        float totalHeight = CalculateTotalHeight();
        return contentRect.position.y + contentRect.size.y * 0.5f - totalHeight * 0.5f;
    }

    case MainAxisAlignment::End:
    {
        float totalHeight = CalculateTotalHeight();
        return contentRect.position.y + contentRect.size.y - totalHeight;
    }

    default:
        return 0.0f;
    }
}

float VerticalLayout::CalculateTotalHeight() const
{
    float totalHeight = 0.0f;

    for (const auto& child : children)
    {
        totalHeight += GetChildLayoutHeight(*child);
    }

    int32 childCount = static_cast<int32>(children.size());
    int32 gapCount = Math::Max(0, childCount - 1);

    totalHeight += gap * gapCount;

    return totalHeight;
}

float VerticalLayout::CalculateChildX(float childWidth, const Rect& contentRect) const
{
    switch (crossAxisAlignment)
    {
    case CrossAxisAlignment::Start:
        return contentRect.position.x;

    case CrossAxisAlignment::Center:
        return contentRect.position.x + contentRect.size.x * 0.5f - childWidth * 0.5f;

    case CrossAxisAlignment::End:
        return contentRect.position.x + contentRect.size.x - childWidth;

    case CrossAxisAlignment::Stretch:
        return contentRect.position.x;

    default:
        return contentRect.position.x;
    }
}

float VerticalLayout::CalculateChildWidth(const Widget& child, const Rect& contentRect) const
{
    switch (crossAxisAlignment)
    {
    case CrossAxisAlignment::Start:
    case CrossAxisAlignment::Center:
    case CrossAxisAlignment::End:
        return GetChildLayoutWidth(child);

    case CrossAxisAlignment::Stretch:
        return contentRect.size.x;

    default:
        return GetChildLayoutWidth(child);
    }
}

float VerticalLayout::GetChildLayoutWidth(const Widget& child) const
{
    return Math::Max(0.0f, child.GetPreferredWidth().value_or(child.GetDesiredSize().x));
}

float VerticalLayout::GetChildLayoutHeight(const Widget& child) const
{
    return Math::Max(0.0f, child.GetPreferredHeight().value_or(child.GetDesiredSize().y));
}

} // namespace URay
