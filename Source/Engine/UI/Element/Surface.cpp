#include "Surface.h"
#include <Core/Math/Math.h>

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
        Math::Max(0.0f, rect.size.x - padding.left - padding.right),
        Math::Max(0.0f, rect.size.y - padding.top - padding.bottom));

    return contentRect;
}

Vector2 Surface::OnMeasure()
{
    Vector2 contentSize = Vector2::Zero;

    for (const auto& child : children)
    {
        const Vector2 childSize = {
            child->GetPreferredWidth().value_or(child->GetDesiredSize().x),
            child->GetPreferredHeight().value_or(child->GetDesiredSize().y),
        };

        contentSize.x = Math::Max(contentSize.x, childSize.x);
        contentSize.y = Math::Max(contentSize.y, childSize.y);
    }

    return contentSize + Vector2(
        padding.left + padding.right,
        padding.top + padding.bottom);
}

void Surface::OnPaint(Render::UIDrawContext& context)
{
    if (backgroundColor.a > 0.0f)
    {
        context.AddFilledRect(rect, backgroundColor);
    }

    if (outlineWidth > 0.0f)
    {
        context.AddOutlineRect(rect, outlineWidth, outlineColor);
    }
}

} // namespace URay
