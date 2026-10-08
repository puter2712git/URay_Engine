#include "VerticalLayout.h"

#include <algorithm>

namespace URay
{

void VerticalLayout::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    switch (mode)
    {
    case VerticalLayoutMode::Top:
        AlignTop();
        break;
    case VerticalLayoutMode::Center:
        AlignCenter();
        break;
    case VerticalLayoutMode::Bottom:
        AlignBottom();
        break;
    }
}

void VerticalLayout::AlignTop()
{
    float y = rect.position.y + padding.top;
    const float width = std::max(0.0f, rect.size.x - padding.left - padding.right);

    for (const auto& child : children)
    {
        const Vector2 desiredSize = child->GetDesiredSize();

        Rect childRect = {};
        childRect.position = Vector2(rect.position.x + padding.left, y);
        childRect.size = Vector2(width, desiredSize.y);

        child->Arrange(childRect);

        y += desiredSize.y + gap;
    }
}

void VerticalLayout::AlignCenter()
{
    float totalHeight = 0.0f;

    for (const auto& child : children)
    {
        totalHeight += child->GetDesiredSize().y;
    }

    const int32 childCount = static_cast<int32>(children.size());
    const int32 gapCount = std::max(0, childCount - 1);
    totalHeight += gap * gapCount;

    float y = rect.position.y + rect.size.y * 0.5f - totalHeight * 0.5f;
    const float width = std::max(0.0f, rect.size.x - padding.left - padding.right);

    for (const auto& child : children)
    {
        const Vector2 desiredSize = child->GetDesiredSize();

        Rect childRect = {};
        childRect.position = Vector2(rect.position.x + padding.left, y);
        childRect.size = Vector2(width, desiredSize.y);

        child->Arrange(childRect);

        y += desiredSize.y + gap;
    }
}

void VerticalLayout::AlignBottom()
{
    float totalHeight = 0.0f;

    for (const auto& child : children)
    {
        totalHeight += child->GetDesiredSize().y;
    }

    const int32 childCount = static_cast<int32>(children.size());
    const int32 gapCount = std::max(0, childCount - 1);
    totalHeight += gap * gapCount;

    float y = rect.position.y + rect.size.y - padding.bottom - totalHeight;
    const float width = std::max(0.0f, rect.size.x - padding.left - padding.right);

    for (const auto& child : children)
    {
        const Vector2 desiredSize = child->GetDesiredSize();

        Rect childRect = {};
        childRect.position = Vector2(rect.position.x + padding.left, y);
        childRect.size = Vector2(width, desiredSize.y);

        child->Arrange(childRect);

        y += desiredSize.y + gap;
    }
}

} // namespace URay
