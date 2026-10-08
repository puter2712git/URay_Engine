#include "HorizontalLayout.h"

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
        const Vector2& childDesiredSize = child->GetDesiredSize();
        float childHeight = CalculateChildHeight(childDesiredSize.y, contentRect);

        Rect childRect;
        childRect.position = Vector2(offsetX, CalculateChildY(childHeight, contentRect));
        childRect.size = Vector2(childDesiredSize.x, childHeight);

        child->Arrange(childRect);

        offsetX += childDesiredSize.x + gap;
    }
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
        totalWidth += child->GetDesiredSize().x;
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

float HorizontalLayout::CalculateChildHeight(float desiredHeight, const Rect& contentRect) const
{
    switch (crossAxisAlignment)
    {
    case CrossAxisAlignment::Start:
    case CrossAxisAlignment::Center:
    case CrossAxisAlignment::End:
        return desiredHeight;

    case CrossAxisAlignment::Stretch:
        return contentRect.size.y;

    default:
        return desiredHeight;
    }
}

} // namespace URay
