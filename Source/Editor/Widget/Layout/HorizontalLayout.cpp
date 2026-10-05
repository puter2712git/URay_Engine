#include "HorizontalLayout.h"

namespace URay
{

void HorizontalLayout::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

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

} // namespace URay
