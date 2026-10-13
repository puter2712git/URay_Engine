#include "UIInputRouter.h"

#include <Core/Input/InputSnapshot.h>
#include <Editor/Widget/Widget.h>
#include <Platform/Window/Window.h>

namespace URay
{

UIInputRouter::UIInputRouter(Window& window)
    : window(window)
{
}

void UIInputRouter::Process(Widget& root, const InputSnapshot& input)
{
    for (const InputEvent& inputEvent : input.GetEvents())
    {
        if (const auto* keyEvent = std::get_if<KeyEvent>(&inputEvent))
        {
            DispatchKeyEvent(root, *keyEvent);
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
            DispatchTextInputEvent(root, *textEvent);
        }
    }
}

bool UIInputRouter::IsTextInputActive(Widget& root) const
{
    Widget* focusedWidget = FindWidgetById(root, focusedWidgetId);
    return focusedWidget && focusedWidget->IsTextInputActive();
}

void UIInputRouter::DispatchKeyEvent(Widget& root, const KeyEvent& event)
{
    Widget* focusedWidget = FindWidgetById(root, focusedWidgetId);
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
    UpdateHoveredWidget(root, hitWidget);

    Widget* pointerCapturedWidget = FindWidgetById(root, pointerCapturedWidgetId);

    Widget* target = pointerCapturedWidget ? pointerCapturedWidget : hitWidget;
    if (!target)
        return;

    EventReply reply = {};

    switch (event.action)
    {
    case PointerAction::Moved:
        BubblePointerEvent(root, target, event, &Widget::OnPointerMove);
        return;

    case PointerAction::Pressed:
        BubblePointerEvent(root, target, event, &Widget::OnPointerDown);
        break;

    case PointerAction::Released:
        BubblePointerEvent(root, target, event, &Widget::OnPointerUp);
        break;
    }
}

void UIInputRouter::DispatchScrollEvent(Widget& root, const ScrollEvent& event)
{
    Widget* target = FindTopWidget(root, event.position);

    for (Widget* widget = target; widget; widget = widget->GetParent())
    {
        const EventReply reply = widget->OnScroll(event);
        ApplyReply(root, widget, reply);

        if (reply.handled)
            return;
    }
}

void UIInputRouter::DispatchTextInputEvent(Widget& root, const TextInputEvent& event)
{
    Widget* focusedWidget = FindWidgetById(root, focusedWidgetId);

    if (focusedWidget)
    {
        ApplyReply(root, focusedWidget, focusedWidget->OnTextInput(event));
    }
}

void UIInputRouter::BubblePointerEvent(Widget& root, Widget* target, const PointerEvent& event, PointerEventHandler handler)
{
    for (Widget* widget = target; widget; widget = widget->GetParent())
    {
        const EventReply reply = (widget->*handler)(event);
        ApplyReply(root, widget, reply);

        if (reply.handled)
            return;
    }
}

void UIInputRouter::UpdateHoveredWidget(Widget& root, Widget* hitWidget)
{
    Widget* hoveredWidget = FindWidgetById(root, hoveredWidgetId);

    if (hitWidget == hoveredWidget)
        return;

    if (hoveredWidget)
    {
        hoveredWidget->hovered = false;
        ApplyReply(root, hoveredWidget, hoveredWidget->OnPointerLeave());
    }

    hoveredWidgetId = hitWidget ? hitWidget->GetId() : InvalidWidgetId;

    if (hitWidget)
    {
        hitWidget->hovered = true;
        ApplyReply(root, hitWidget, hitWidget->OnPointerEnter());
    }
}

void UIInputRouter::ApplyReply(Widget& root, Widget* target, const EventReply& reply)
{
    if (reply.capturePointer)
    {
        pointerCapturedWidgetId = target->GetId();
        target->pointerCaptured = true;
    }

    if (reply.releasePointer)
    {
        if (Widget* captured = FindWidgetById(root, pointerCapturedWidgetId))
        {
            captured->pointerCaptured = false;
        }

        pointerCapturedWidgetId = InvalidWidgetId;
    }

    if (reply.requestFocus && focusedWidgetId != target->GetId())
    {
        if (Widget* previous = FindWidgetById(root, focusedWidgetId))
        {
            previous->focused = false;
        }

        focusedWidgetId = target->GetId();
        target->focused = true;
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

    if (!root.CanHit())
        return nullptr;

    return &root;
}

Widget* UIInputRouter::FindWidgetById(Widget& root, WidgetId id) const
{
    if (id == InvalidWidgetId)
        return nullptr;

    if (root.GetId() == id)
        return &root;

    for (const auto& child : root.GetChildren())
    {
        if (Widget* found = FindWidgetById(*child, id))
            return found;
    }

    return nullptr;
}

} // namespace URay
