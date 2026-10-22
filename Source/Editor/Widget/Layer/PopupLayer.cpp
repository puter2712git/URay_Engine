#include "PopupLayer.h"
#include <Core/Math/Math.h>

namespace URay
{

void PopupLayer::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    if (!activePopup)
        return;

    const Vector2 desiredSize = activePopup->GetDesiredSize();

    Rect popupRect;
    popupRect.position = Vector2(
        anchorRect.position.x,
        anchorRect.position.y + anchorRect.size.y);
    popupRect.size = Vector2(
        Math::Max(anchorRect.size.x, desiredSize.x),
        desiredSize.y);

    // Clamping
    popupRect.position.x = Math::Min(
        popupRect.position.x,
        rect.position.x + rect.size.x - popupRect.size.x);
    popupRect.position.y = Math::Min(
        popupRect.position.y,
        rect.position.y + rect.size.y - popupRect.size.y);

    activePopup->Arrange(popupRect);
}

EventReply PopupLayer::OnPointerDown(const PointerEvent& event)
{
    ClosePopup();

    return EventReply{
        .handled = true
    };
}

void PopupLayer::ClosePopup()
{
    closeRequested = true;
}

void PopupLayer::OnUpdate(float deltaTime)
{
    if (!closeRequested)
        return;

    closeRequested = false;
    ClearChildren();
    activePopup = nullptr;
}

} // namespace URay
