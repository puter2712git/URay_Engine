#include "Widget.h"

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

} // namespace URay
