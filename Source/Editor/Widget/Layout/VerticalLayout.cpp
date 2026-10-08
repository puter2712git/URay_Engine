#include "VerticalLayout.h"

#include <algorithm>

namespace URay
{

void VerticalLayout::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    Rect contentRect;
    contentRect.position = Vector2(rect.position.x + padding.left, rect.position.y + padding.top);
    contentRect.size = {
        std::max(0.0f, rect.size.x - padding.left - padding.right),
        std::max(0.0f, rect.size.y - padding.top - padding.bottom),
    };

    float offsetY = CalculateStartOffsetY(contentRect);

    for (const auto& child : children)
    {
        const Vector2& childDesiredSize = child->GetDesiredSize();
        float childWidth = CalculateChildWidth(childDesiredSize.x, contentRect);

        Rect childRect;
        childRect.position = Vector2(CalculateChildX(childWidth, contentRect), offsetY);
        childRect.size = Vector2(childWidth, childDesiredSize.y);

        child->Arrange(childRect);

        offsetY += childDesiredSize.y + gap;
    }
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
        totalHeight += child->GetDesiredSize().y;
    }

    int32 childCount = static_cast<int32>(children.size());
    int32 gapCount = std::max(0, childCount - 1);

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

float VerticalLayout::CalculateChildWidth(float desiredWidth, const Rect& contentRect) const
{
    switch (crossAxisAlignment)
    {
    case CrossAxisAlignment::Start:
    case CrossAxisAlignment::Center:
    case CrossAxisAlignment::End:
        return desiredWidth;

    case CrossAxisAlignment::Stretch:
        return contentRect.size.x;

    default:
        return desiredWidth;
    }
}

} // namespace URay
