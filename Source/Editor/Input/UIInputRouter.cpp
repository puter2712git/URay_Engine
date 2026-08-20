#include "UIInputRouter.h"

#include "Editor/Widget/Widget.h"

#include "Core/Input/InputManager.h"

#include "Platform/Window/Window.h"

namespace URay
{

UIInputRouter::UIInputRouter(Window& window)
    : window(window)
{
}

void UIInputRouter::Process(Widget& root, const InputManager& input)
{
    for (const InputEvent& inputEvent : input.GetEvents())
    {
        if (const auto* keyEvent = std::get_if<KeyEvent>(&inputEvent))
        {
            DispatchKeyEvent(*keyEvent);
        }
        else if (const auto* pointerEvent = std::get_if<PointerEvent>(&inputEvent))
        {
            DispatchPointerEvent(root, *pointerEvent);
        }
        else if (const auto* scrollEvent = std::get_if<ScrollEvent>(&inputEvent))
        {
            DispatchScrollEvent(root, *scrollEvent);
        }
        else if (const auto* textEvent = std::get_if<TextInputEvent>(&inputEvent))
        {
            DispatchTextInputEvent(*textEvent);
        }
    }
}

void UIInputRouter::DispatchKeyEvent(const KeyEvent& event)
{
    if (!focusedWidget)
        return;

    if (event.action == KeyAction::Released)
    {
        focusedWidget->OnKeyUp(event);
    }
    else
    {
        focusedWidget->OnKeyDown(event);
    }
}

void UIInputRouter::DispatchPointerEvent(Widget& root, const PointerEvent& event)
{
    Widget* hitWidget = FindTopWidget(root, event.position);
    UpdateHoveredWidget(hitWidget);

    Widget* target = pointerCapturedWidget ? pointerCapturedWidget : hitWidget;
    if (!target)
        return;

    EventReply reply = {};

    switch (event.action)
    {
    case PointerAction::Moved:
        target->OnPointerMove(event);
        return;

    case PointerAction::Pressed:
        reply = target->OnPointerDown(event);
        break;

    case PointerAction::Released:
        reply = target->OnPointerUp(event);
        break;
    }

    ApplyReply(target, reply);
}

void UIInputRouter::DispatchScrollEvent(Widget& root, const ScrollEvent& event)
{
}

void UIInputRouter::DispatchTextInputEvent(const TextInputEvent& event)
{
}

void UIInputRouter::UpdateHoveredWidget(Widget* hitWidget)
{
    if (hitWidget == hoveredWidget)
        return;

    if (hoveredWidget)
    {
        hoveredWidget->hovered = false;
        ApplyReply(hoveredWidget, hoveredWidget->OnPointerLeave());
    }

    hoveredWidget = hitWidget;

    if (hoveredWidget)
    {
        hoveredWidget->hovered = true;
        ApplyReply(hoveredWidget, hoveredWidget->OnPointerEnter());
    }
}

void UIInputRouter::ApplyReply(Widget* target, const EventReply& reply)
{
    if (reply.capturePointer)
    {
        pointerCapturedWidget = target;
        target->pointerCaptured = true;
    }

    if (reply.releasePointer && pointerCapturedWidget)
    {
        pointerCapturedWidget->pointerCaptured = false;
        pointerCapturedWidget = nullptr;
    }

    if (reply.requestFocus && focusedWidget != target)
    {
        if (focusedWidget)
        {
            focusedWidget->focused = false;
        }

        focusedWidget = target;
        focusedWidget->focused = true;
    }

    if (reply.cursor.has_value())
    {
        window.ChangeCursor(*reply.cursor);
    }
}

Widget* UIInputRouter::FindTopWidget(Widget& root, const Vector2& position) const
{
    if (!root.HitTest(position))
        return nullptr;

    const auto& children = root.GetChildren();

    for (auto it = children.rbegin(); it != children.rend(); ++it)
    {
        Widget* child = it->get();

        if (Widget* hit = FindTopWidget(*child, position))
            return hit;
    }

    return &root;
}

} // namespace URay
