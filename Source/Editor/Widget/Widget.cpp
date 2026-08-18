#include "Widget.h"

#include <imgui/imgui.h>

namespace URay
{

void Widget::Update()
{
    OnUpdate();

    for (const auto& child : children)
    {
        child->Update();
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

void Widget::AddChild(std::unique_ptr<Widget> child)
{
    children.push_back(std::move(child));
}

void Widget::ApplyRect() const
{
    ImGui::SetNextWindowPos(ImVec2(rect.position.x, rect.position.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(rect.size.x, rect.size.y), ImGuiCond_Always);
}

} // namespace URay
