#pragma once

#include "Editor/Widget/Widget.h"

#include <memory>

namespace URay
{

enum class SplitAxis
{
    Horizontal,
    Vertical
};

class Splitter : public Widget
{
public:
    Splitter(SplitAxis axis, std::unique_ptr<Widget> first, std::unique_ptr<Widget> second);
    ~Splitter() override;

public:
    EventReply OnPointerEnter() override;
    EventReply OnPointerLeave() override;
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float) override;

private:
    SplitAxis axis = SplitAxis::Horizontal;

    float splitRatio = 0.5f;
    float handleThickness = 4.0f;

    bool isDragging = false;
};

} // namespace URay
