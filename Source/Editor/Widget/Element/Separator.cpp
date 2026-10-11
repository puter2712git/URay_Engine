#include "Separator.h"

namespace URay
{

Separator::Separator(SeparatorOrientation orientation) : orientation(orientation) {}

Vector2 Separator::OnMeasure()
{
    if (orientation == SeparatorOrientation::Horizontal)
    {
        return Vector2(
            0.0f,
            padding.top + thickness + padding.bottom);
    }
    else
    {
        return Vector2(
            padding.left + thickness + padding.right,
            0.0f);
    }
}

void Separator::OnPaint(Render::UIDrawContext& context)
{
    if (orientation == SeparatorOrientation::Horizontal)
    {
        Rect lineRect;
        lineRect.position = Vector2(
            rect.position.x,
            rect.position.y + (rect.size.y - thickness) * 0.5f);
        lineRect.size = Vector2(rect.size.x, thickness);

        context.AddFilledRect(lineRect, color);
    }
    else
    {
        Rect lineRect;
        lineRect.position = Vector2(
            rect.position.x + (rect.size.x - thickness) * 0.5f,
            rect.position.y);
        lineRect.size = Vector2(thickness, rect.size.y);

        context.AddFilledRect(lineRect, color);
    }
}

} // namespace URay
