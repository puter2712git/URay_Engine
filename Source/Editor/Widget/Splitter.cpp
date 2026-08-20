#include "Splitter.h"

#include <algorithm>

namespace URay
{

Splitter::Splitter(SplitAxis axis, std::unique_ptr<Widget> first, std::unique_ptr<Widget> second)
    : axis(axis)
{
    AddChild(std::move(first));
    AddChild(std::move(second));
}

Splitter::~Splitter() = default;

EventReply Splitter::OnPointerEnter()
{
    return {
        .cursor = axis == SplitAxis::Horizontal
                      ? CursorType::HRESIZE
                      : CursorType::VRESIZE
    };
}

EventReply Splitter::OnPointerLeave()
{
    return {
        .cursor = CursorType::ARROW
    };
}

EventReply Splitter::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Left)
    {
        isDragging = true;

        return EventReply{
            .capturePointer = true,
        };
    }

    return {};
}

EventReply Splitter::OnPointerMove(const PointerEvent& event)
{
    if (isDragging)
    {
        float delta = axis == SplitAxis::Horizontal
                          ? event.delta.x
                          : event.delta.y;
        splitRatio += delta * 0.001f;
    }

    return {};
}

EventReply Splitter::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Left && isDragging)
    {
        isDragging = false;

        return EventReply{
            .releasePointer = true,
        };
    }

    return {};
}

void Splitter::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    if (children.size() != 2)
        return;

    Widget* first = children[0].get();
    Widget* second = children[1].get();

    const bool isHorizontal = axis == SplitAxis::Horizontal;

    const float totalLength = isHorizontal ? rect.size.x : rect.size.y;
    const float actualHandleThickness = std::min(handleThickness, std::max(0.0f, totalLength));

    const float contentLength = std::max(0.0f, totalLength - actualHandleThickness);
    const float ratio = std::clamp(splitRatio, 0.0f, 1.0f);

    const float firstLength = contentLength * ratio;
    const float secondLength = contentLength - firstLength;

    Rect firstRect = {};
    Rect secondRect = {};

    if (isHorizontal)
    {
        firstRect.position = rect.position;
        firstRect.size = Vector2(firstLength, rect.size.y);

        secondRect.position = Vector2(
            rect.position.x + firstLength + actualHandleThickness,
            rect.position.y);
        secondRect.size = Vector2(secondLength, rect.size.y);
    }
    else
    {
        firstRect.position = rect.position;
        firstRect.size = Vector2(rect.size.x, firstLength);

        secondRect.position = Vector2(
            rect.position.x,
            rect.position.y + firstLength + actualHandleThickness);
        secondRect.size = Vector2(rect.size.x, secondLength);
    }

    first->Arrange(firstRect);
    second->Arrange(secondRect);
}

void Splitter::OnUpdate(float)
{
}

} // namespace URay
