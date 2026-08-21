#pragma once

#include "Editor/Widget/Widget.h"

#include <memory>
#include <string>

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
    Splitter(const std::string& layoutId, SplitAxis axis, std::unique_ptr<Widget> first, std::unique_ptr<Widget> second);
    ~Splitter() override;

public:
    EventReply OnPointerEnter() override;
    EventReply OnPointerLeave() override;
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    void Arrange(const Rect& rect) override;

    float GetSplitRatio() const { return splitRatio; }
    void SetSplitRatio(float ratio) { splitRatio = ratio; }

    const std::string& GetLayoutId() const { return layoutId; }

protected:
    void OnUpdate(float) override;

private:
    std::string layoutId;

    SplitAxis axis = SplitAxis::Horizontal;

    float splitRatio = 0.5f;
    float handleThickness = 4.0f;

    bool isDragging = false;

    float dragStartPosition = 0.0f;
    float dragStartRatio = 0.0f;
};

} // namespace URay
