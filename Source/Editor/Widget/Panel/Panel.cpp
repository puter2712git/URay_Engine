#include "Panel.h"

#include "Editor/Widget/Panel/PanelHeader.h"

#include "Render/Rendering/Batch/UIDrawContext.h"

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
        headerRect.size = Vector2(rect.size.x, 25.0f);

        header->Arrange(headerRect);
    }
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
