#include "Widget.h"

#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay
{

URAY_REGISTER_CLASS(Widget)

void Widget::RegisterClass() {}

Widget::Widget() {}

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

std::unique_ptr<Widget> Widget::RemoveChild(Widget* child)
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if (it->get() != child)
            continue;

        std::unique_ptr<Widget> removed = std::move(*it);
        children.erase(it);

        removed->parent = nullptr;
        return removed;
    }

    return nullptr;
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
