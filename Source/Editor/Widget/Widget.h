#pragma once

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

protected:
    virtual void OnUpdate() {}
    virtual void OnDraw() {}

protected:
    std::vector<std::unique_ptr<Widget>> children;
};

} // namespace URay
