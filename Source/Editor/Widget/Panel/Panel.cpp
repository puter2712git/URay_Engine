#include "Panel.h"

#include <Editor/Widget/Style/EditorStyles.h>
#include <Core/Math/Math.h>

#include <Editor/Widget/Panel/PanelHeader.h>

#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay
{

Panel::Panel() = default;

Panel::Panel(const std::string& name)
{
    header = AddChild<PanelHeader>(name);
}

Panel::~Panel() = default;

void Panel::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    if (header)
    {
        Rect headerRect = {};
        headerRect.position = rect.position;
        headerRect.size = Vector2(rect.size.x, EditorStyle::PanelHeaderHeight);

        header->Arrange(headerRect);
    }
}

Rect Panel::GetContentRect() const
{
    const float effectiveOutline = Math::Max(0.0f, Math::Min(outlineWidth, Math::Min(rect.size.x, rect.size.y) * 0.5f));

    Rect contentRect = rect;

    const float left = effectiveOutline + padding.left;
    const float top = effectiveOutline + padding.top;
    const float right = effectiveOutline + padding.right;
    const float bottom = effectiveOutline + padding.bottom;

    contentRect.position += Vector2(left, top);
    contentRect.size = Vector2(
        Math::Max(0.0f, rect.size.x - left - right),
        Math::Max(0.0f, rect.size.y - top - bottom));

    if (header)
    {
        const float headerHeight = Math::Min(EditorStyle::PanelHeaderHeight, contentRect.size.y);

        contentRect.position.y += headerHeight;
        contentRect.size.y -= headerHeight;
    }

    return contentRect;
}

void Panel::OnPaint(Render::UIDrawContext& context)
{
    context.AddFilledRect(rect, backgroundColor);
}

void Panel::OnPaintForeground(Render::UIDrawContext& context)
{
    if (outlineWidth > 0.0f)
    {
        context.AddOutlineRect(rect, outlineWidth, outlineColor);
    }
}

} // namespace URay
