#include "ScrollView.h"
#include <Core/Math/Math.h>

#include <algorithm>

namespace URay
{

void ScrollView::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    if (!content)
        return;

    const float contentHeight =
        content->GetPreferredHeight().value_or(content->GetDesiredSize().y);

    maxScrollOffsetY = Math::Max(0.0f, contentHeight - rect.size.y);

    if (scrollToBottomRequested)
    {
        scrollOffsetY = maxScrollOffsetY;
        scrollToBottomRequested = false;
    }
    else
    {
        scrollOffsetY = Math::Clamp(scrollOffsetY, 0.0f, maxScrollOffsetY);
    }

    Rect contentRect;
    contentRect.position = Vector2(rect.position.x, rect.position.y - scrollOffsetY);
    contentRect.size = Vector2(rect.size.x, Math::Max(rect.size.y, contentHeight));

    content->Arrange(contentRect);
}

EventReply ScrollView::OnScroll(const ScrollEvent& event)
{
    constexpr float ScrollPixelsPerUnit = 40.0f;

    const float previousOffset = scrollOffsetY;

    scrollOffsetY = Math::Clamp(
        scrollOffsetY - event.delta.y * ScrollPixelsPerUnit,
        0.0f,
        maxScrollOffsetY);

    return EventReply{
        .handled = scrollOffsetY != previousOffset
    };
}

Vector2 ScrollView::OnMeasure()
{
    return content ? content->GetDesiredSize() : Vector2::Zero;
}

} // namespace URay
