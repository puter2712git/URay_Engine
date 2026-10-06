#include "HorizontalLayout.h"

namespace URay
{

void HorizontalLayout::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    switch (mode)
    {
    case HorizontalLayoutMode::Left:
        AlignLeft();
        break;
    case HorizontalLayoutMode::Center:
        AlignCenter();
        break;
    case HorizontalLayoutMode::Right:
        AlignRight();
        break;
    }
}

void HorizontalLayout::AlignLeft()
{
    float x = rect.position.x + padding.left;

    for (const auto& child : children)
    {
        Rect childRect = {};
        childRect.position = Vector2(x, rect.position.x + padding.top);
        childRect.size = Vector2(
            child->GetDesiredSize().x,
            rect.size.y - padding.top - padding.bottom);

        child->Arrange(childRect);

        x += child->GetDesiredSize().x + gap;
    }
}

void HorizontalLayout::AlignCenter()
{
    float totalWidth = 0;

    for (const auto& child : children)
    {
        totalWidth += child->GetDesiredSize().x;
    }

    int32 childCount = static_cast<int32>(children.size());
    int32 gapCount = std::max(0, childCount - 1);
    totalWidth += gap * gapCount;

    float xOffset = rect.position.x + rect.size.x * 0.5f - totalWidth * 0.5f;

    for (const auto& child : children)
    {
        const Vector2 desiredSize = child->GetDesiredSize();

        Rect childRect = {};
        childRect.position = Vector2(xOffset, rect.position.y + padding.top);
        childRect.size = desiredSize;

        child->Arrange(childRect);

        xOffset += desiredSize.x + gap;
    }
}

void HorizontalLayout::AlignRight()
{
}

} // namespace URay
