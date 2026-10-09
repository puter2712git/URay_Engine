#include "Splitter.h"

#include <Editor/Widget/Style/EditorStyles.h>

#include <Core/Type/Types.h>

#include <Render/Rendering/Batch/UIDrawContext.h>

#include <algorithm>

namespace URay
{

Splitter::Splitter(const std::string& layoutId, SplitAxis axis, std::unique_ptr<Widget> first, std::unique_ptr<Widget> second)
    : layoutId(layoutId), axis(axis)
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
    CursorType cursorType = axis == SplitAxis::Horizontal
                                ? CursorType::HRESIZE
                                : CursorType::VRESIZE;

    return {
        .cursor = isDragging ? cursorType : CursorType::ARROW
    };
}

EventReply Splitter::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || !IsHandleHit(event.position))
        return {};

    isDragging = true;

    dragStartPosition = axis == SplitAxis::Horizontal ? event.position.x : event.position.y;
    dragStartRatio = splitRatio;

    return EventReply{
        .handled = true,
        .capturePointer = true
    };
}

EventReply Splitter::OnPointerMove(const PointerEvent& event)
{
    if (!isDragging)
        return {};

    const float currPosition = axis == SplitAxis::Horizontal ? event.position.x : event.position.y;
    const float movedPixels = currPosition - dragStartPosition;

    uint32 contentLength = axis == SplitAxis::Horizontal ? rect.size.x : rect.size.y;

    const float firstLength = dragStartRatio * contentLength + movedPixels;
    splitRatio = std::clamp(firstLength / contentLength, 0.0f, 1.0f);

    return EventReply{ .handled = true };
}

EventReply Splitter::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || !isDragging)
        return {};

    isDragging = false;

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
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

void Splitter::OnPaint(Render::UIDrawContext& context)
{
    Rect handleRect = GetHandleRect();

    const Color handleColor = (IsHovered() || isDragging) ? EditorStyle::ControlHovered : EditorStyle::ControlNormal;

    context.AddFilledRect(handleRect, handleColor);
}

bool Splitter::IsHandleHit(const Vector2& position) const
{
    const Rect handle = GetHandleRect();

    return position.x >= handle.position.x &&
           position.x < handle.position.x + handle.size.x &&
           position.y >= handle.position.y &&
           position.y < handle.position.y + handle.size.y;
}

Rect Splitter::GetHandleRect() const
{
    const bool isHorizontal = axis == SplitAxis::Horizontal;

    const float totalLength = isHorizontal ? rect.size.x : rect.size.y;
    const float actualHandleThickness = std::min(handleThickness, std::max(0.0f, totalLength));
    const float contentLength = std::max(0.0f, totalLength - actualHandleThickness);
    const float firstLength = contentLength * std::clamp(splitRatio, 0.0f, 1.0f);

    Rect handleRect = {};

    if (isHorizontal)
    {
        handleRect.position = Vector2(
            rect.position.x + firstLength,
            rect.position.y);
        handleRect.size = Vector2(
            actualHandleThickness,
            rect.size.y);
    }
    else
    {
        handleRect.position = Vector2(
            rect.position.x,
            rect.position.y + firstLength);
        handleRect.size = Vector2(
            rect.size.x,
            actualHandleThickness);
    }

    return handleRect;
}

} // namespace URay
