#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/UI/Widget/Widget.h>

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
    URAY_CLASS(Splitter, Widget)

public:
    Splitter(const String& layoutId, SplitAxis axis, std::unique_ptr<Widget> first, std::unique_ptr<Widget> second);
    Splitter(const String& layoutId, SplitAxis axis);
    ~Splitter() override;

public:
    EventReply OnPointerEnter() override;
    EventReply OnPointerLeave() override;
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;
    void OnPointerCancel() override;

    void Arrange(const Rect& rect) override;

    SplitAxis GetAxis() const { return axis; }
    float GetSplitRatio() const { return splitRatio; }
    void SetSplitRatio(float ratio) { splitRatio = ratio; }

    const String& GetLayoutId() const { return layoutId; }

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    bool IsHandleHit(const Vector2& position) const;
    Rect GetHandleRect() const;

private:
    String layoutId;

    SplitAxis axis = SplitAxis::Horizontal;

    float splitRatio = 0.5f;
    float handleThickness = 4.0f;

    bool isDragging = false;

    float dragStartPosition = 0.0f;
    float dragStartRatio = 0.0f;
};

} // namespace URay
