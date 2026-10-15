#include "Widget.h"

#include <Render/Rendering/Batch/UIDrawContext.h>

#include <atomic>

namespace URay
{

std::atomic<WidgetId> nextWidgetId = 1;

Widget::Widget() : id(nextWidgetId.fetch_add(1, std::memory_order_relaxed)) {}

void Widget::Update(float deltaTime)
{
    OnUpdate(deltaTime);

    for (const auto& child : children)
    {
        child->Update(deltaTime);
    }
}

void Widget::Paint(Render::UIDrawContext& context)
{
    const std::optional<Rect> clipRect = GetClipRect();
    if (clipRect)
        context.PushClipRect(*clipRect);

    OnPaint(context);

    for (const auto& child : children)
    {
        child->Paint(context);
    }

    OnPaintForeground(context);

    if (clipRect)
        context.PopClipRect();
}

void Widget::AddChild(std::unique_ptr<Widget> child)
{
    child->parent = this;
    children.push_back(std::move(child));
}

void Widget::Measure()
{
    for (const auto& child : children)
    {
        child->Measure();
    }

    desiredSize = OnMeasure();
}

bool Widget::HitTest(const Vector2& position) const
{
    return position.x >= rect.position.x && position.x < rect.position.x + rect.size.x &&
           position.y >= rect.position.y && position.y < rect.position.y + rect.size.y;
}

} // namespace URay
