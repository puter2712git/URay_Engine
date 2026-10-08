#include "Panel.h"

#include "Editor/Widget/Panel/PanelHeader.h"

#include "Render/Rendering/Batch/UIDrawContext.h"

namespace URay
{

Panel::Panel() = default;

Panel::Panel(const std::string& name)
{
    auto panelHeader = std::make_unique<PanelHeader>(name);
    header = panelHeader.get();

    AddChild(std::move(panelHeader));
}

Panel::~Panel() = default;

void Panel::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    if (header)
    {
        Rect headerRect = {};
        headerRect.position = rect.position;
        headerRect.size = Vector2(rect.size.x, 20.0f);

        header->Arrange(headerRect);
    }
}

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
