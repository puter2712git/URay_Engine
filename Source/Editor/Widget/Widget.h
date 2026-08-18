#pragma once

#include "Core/Math/Rect.h"

#include <memory>
#include <vector>

namespace URay
{

class Widget
{
public:
    virtual ~Widget() = default;

public:
    void Update();
    void Draw();

    void AddChild(std::unique_ptr<Widget> child);

    virtual void Arrange(const Rect& rect) { this->rect = rect; }
    const Rect& GetRect() const { return rect; }

protected:
    virtual void OnUpdate() {}
    virtual void OnDraw() {}

protected:
    std::vector<std::unique_ptr<Widget>> children;

    Rect rect = {};
};

} // namespace URay
