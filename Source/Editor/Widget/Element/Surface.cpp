#include "Surface.h"

#include <Render/Rendering/Batch/UIDrawContext.h>

#include <algorithm>

namespace URay
{

void Surface::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    const Rect contentRect = GetContentRect();
    for (const auto& child : children)
    {
        child->Arrange(contentRect);
    }
}

Rect Surface::GetContentRect() const
{
    Rect contentRect = rect;
    contentRect.position += Vector2(padding.left, padding.top);
    contentRect.size = Vector2(
        std::max(0.0f, rect.size.x - padding.left - padding.right),
        std::max(0.0f, rect.size.y - padding.top - padding.bottom));

    return contentRect;
}

void Surface::OnPaint(Render::UIDrawContext& context)
{
    if (backgroundColor.a > 0.0f)
    {
        context.AddFilledRect(rect, backgroundColor);
    }
}

} // namespace URay
