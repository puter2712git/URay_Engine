#include "DialogLayer.h"

namespace URay
{

void DialogLayer::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    if (!activeDialog)
        return;

    const Vector2 desiredSize = activeDialog->GetDesiredSize();

    Rect dialogRect;
    dialogRect.position = Vector2(
        rect.position.x + (rect.size.x - desiredSize.x) * 0.5f,
        rect.position.y + (rect.size.y - desiredSize.y) * 0.5f);
    dialogRect.size = desiredSize;

    activeDialog->Arrange(dialogRect);
}

EventReply DialogLayer::OnPointerDown(const PointerEvent& event)
{
    return EventReply{
        .handled = true
    };
}

void DialogLayer::CloseDialog()
{
    closeRequested = true;
}

void DialogLayer::OnUpdate(float deltaTime)
{
    if (!closeRequested)
        return;

    closeRequested = false;
    ClearChildren();
    activeDialog = nullptr;
}

} // namespace URay
