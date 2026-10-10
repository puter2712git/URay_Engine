#include "Widget.h"

#include <Render/Rendering/Batch/UIDrawContext.h>

#include <imgui/imgui.h>

namespace URay
{

void Widget::Update(float deltaTime)
{
    OnUpdate(deltaTime);

    for (const auto& child : children)
    {
        child->Update(deltaTime);
    }
}

void Widget::Draw()
{
    OnDraw();

    for (const auto& child : children)
    {
        child->Draw();
    }
}

void Widget::Paint(Render::UIDrawContext& context)
{
    context.PushClipRect(rect);

    OnPaint(context);

    for (const auto& child : children)
    {
        child->Paint(context);
    }

    OnPaintForeground(context);

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

void Widget::ApplyRect() const
{
    ImGui::SetNextWindowPos(ImVec2(rect.position.x, rect.position.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(rect.size.x, rect.size.y), ImGuiCond_Always);
}

} // namespace URay
