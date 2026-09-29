#include "Panel.h"

#include "Render/Rendering/Batch/UIDrawContext.h"

namespace URay
{

Panel::Panel() = default;

Panel::~Panel() = default;

void Panel::OnPaint(Render::UIDrawContext& context)
{
    if (outlineWidth > 0.0f)
    {
        context.AddFilledRect(rect, outlineColor);

        Rect innerRect = rect;
        innerRect.position += Vector2(outlineWidth, outlineWidth);
        innerRect.size -= Vector2(outlineWidth * 2.0f, outlineWidth * 2.0f);

        context.AddFilledRect(innerRect, backgroundColor);
        return;
    }

    context.AddFilledRect(rect, backgroundColor);
}

} // namespace URay
