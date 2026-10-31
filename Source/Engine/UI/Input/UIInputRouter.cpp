#include "UIInputRouter.h"

#include <Core/Input/InputSnapshot.h>
#include <Engine/UI/UISystem.h>
#include <Engine/UI/Widget/Widget.h>
#include <Platform/Window/Window.h>

namespace URay
{

namespace
{

bool IsInSubtree(const Widget* widget, const Widget& subtree)
{
    for (const Widget* current = widget; current; current = current->GetParent())
    {
        if (current == &subtree)
            return true;
    }
    return false;
}

} // namespace

UIInputRouter::UIInputRouter(UISystem& uiSystem) : uiSystem(uiSystem) {}

void UIInputRouter::Process(Window& window, Widget& root, const InputSnapshot& input)
{
    for (const InputEvent& event : input.GetEvents())
    {
        ProcessEvent(window, root, event);
    }
}

void UIInputRouter::ProcessEvent(Window& window, Widget& root, const InputEvent& event)
{
    if (const auto* keyEvent = std::get_if<KeyEvent>(&event))
    {
        DispatchKeyEvent(root, *keyEvent);
    }
    else if (const auto* pointerEvent = std::get_if<PointerEvent>(&event))
    {
        DispatchPointerEvent(window, root, *pointerEvent);
    }
    else if (const auto* scrollEvent = std::get_if<ScrollEvent>(&event))
    {
        DispatchScrollEvent(window, root, *scrollEvent);
    }
    else if (const auto* textEvent = std::get_if<TextInputEvent>(&event))
    {
        DispatchTextInputEvent(window, root, *textEvent);
    }
    else if (const auto* focusEvent = std::get_if<WindowFocusEvent>(&event))
    {
        if (!focusEvent->focused)
        {
            CancelCaptureForWindow(window);
        }
    }
}

bool UIInputRouter::IsTextInputActive(Widget& root) const
{
    Widget* focusedWidget = FindWidgetById(root, focusedTarget.widgetId);
    return focusedWidget && focusedWidget->IsTextInputActive();
}

bool UIInputRouter::TryClearInputStateForWidget(Window& window, Widget& root, Widget& widget)
{
    return TryClearInputState(window, root, widget, false);
}

bool UIInputRouter::TryClearInputStateForSubtree(Window& window, Widget& root, Widget& subtree)
{
    return TryClearInputState(window, root, subtree, true);
}

void UIInputRouter::ClearHoverForWindow(Window& window)
{
    if (hoveredTarget.window != &window)
        return;

    Widget* hovered = FindTargetWidget(hoveredTarget);
    hoveredTarget = {};

    if (!hovered)
        return;

    hovered->hovered = false;

    const EventReply reply = hovered->OnPointerLeave();
    if (reply.cursor.has_value())
        window.ChangeCursor(*reply.cursor);
}

void UIInputRouter::ClearFocusForWindow(Window& window)
{
    if (focusedTarget.window != &window)
        return;

    if (Widget* focused = FindTargetWidget(focusedTarget))
    {
        focused->focused = false;
    }

    focusedTarget = {};
}

void UIInputRouter::CancelCaptureForWindow(Window& window)
{
    if (capturedTarget.window != &window)
        return;

    Widget* captured = FindTargetWidget(capturedTarget);
    capturedTarget = {};

    if (captured)
    {
        captured->pointerCaptured = false;
        captured->OnPointerCancel();
    }
}

void UIInputRouter::RestoreFocusForWindow(Window& window)
{
    Widget* root = uiSystem.FindWindowRoot(window);
    if (!root)
        return;

    const ObjectId widgetId = uiSystem.GetLastFocusedWidget(window);
    Widget* candidate = FindWidgetById(*root, widgetId);

    if (!candidate)
    {
        uiSystem.SetLastFocusedWidget(window, {});
        return;
    }

    for (Widget* current = candidate; current; current = current->GetParent())
    {
        if (!current->IsVisible())
            return;
    }

    if (focusedTarget.window == &window && focusedTarget.widgetId == widgetId)
        return;

    if (Widget* previous = FindTargetWidget(focusedTarget))
        previous->focused = false;

    focusedTarget = {
        .window = &window,
        .widgetId = widgetId
    };
    candidate->focused = true;
}

Widget* UIInputRouter::FindTopWidget(Widget& root, const Vector2& position) const
{
    if (!root.IsVisible() || !root.HitTest(position))
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

void UIInputRouter::DispatchKeyEvent(Widget& root, const KeyEvent& event)
{
    Widget* focusedWidget = FindWidgetById(root, focusedTarget.widgetId);
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

void UIInputRouter::DispatchPointerEvent(Window& window, Widget& root, const PointerEvent& event)
{
    Widget* hitWidget = FindTopWidget(root, event.position);
    UpdateHoveredWidget(window, root, hitWidget);

    Widget* pointerCapturedWidget = nullptr;

    if (capturedTarget.window == &window)
    {
        pointerCapturedWidget = FindWidgetById(root, capturedTarget.widgetId);
    }

    Widget* target = pointerCapturedWidget ? pointerCapturedWidget : hitWidget;
    if (!target)
        return;

    EventReply reply = {};

    switch (event.action)
    {
    case PointerAction::Moved:
        BubblePointerEvent(window, root, target, event, &Widget::OnPointerMove);
        return;

    case PointerAction::Pressed:
        BubblePointerEvent(window, root, target, event, &Widget::OnPointerDown);
        break;

    case PointerAction::Released:
        BubblePointerEvent(window, root, target, event, &Widget::OnPointerUp);
        break;
    }
}

void UIInputRouter::DispatchScrollEvent(Window& window, Widget& root, const ScrollEvent& event)
{
    Widget* target = FindTopWidget(root, event.position);

    for (Widget* widget = target; widget; widget = widget->GetParent())
    {
        const EventReply reply = widget->OnScroll(event);
        ApplyReply(window, root, widget, reply);

        if (reply.handled)
            return;
    }
}

void UIInputRouter::DispatchTextInputEvent(Window& window, Widget& root, const TextInputEvent& event)
{
    Widget* focusedWidget = FindWidgetById(root, focusedTarget.widgetId);

    if (focusedWidget)
    {
        ApplyReply(window, root, focusedWidget, focusedWidget->OnTextInput(event));
    }
}

void UIInputRouter::BubblePointerEvent(Window& window, Widget& root, Widget* target, const PointerEvent& event, PointerEventHandler handler)
{
    for (Widget* widget = target; widget; widget = widget->GetParent())
    {
        const EventReply reply = (widget->*handler)(event);
        ApplyReply(window, root, widget, reply);

        if (reply.handled)
            return;
    }
}

void UIInputRouter::UpdateHoveredWidget(Window& window, Widget& root, Widget* hitWidget)
{
    Widget* previous = FindTargetWidget(hoveredTarget);

    if (hoveredTarget.window == &window && previous == hitWidget)
        return;

    if (previous)
    {
        Window* previousWindow = hoveredTarget.window;
        Widget* previousRoot = uiSystem.FindWindowRoot(*previousWindow);

        previous->hovered = false;

        if (previousRoot)
        {
            const EventReply reply = previous->OnPointerLeave();
            ApplyReply(*previousWindow, *previousRoot, previous, reply);
        }
    }

    hoveredTarget = {};

    if (hitWidget)
    {
        hoveredTarget = {
            .window = &window,
            .widgetId = hitWidget->GetId()
        };

        hitWidget->hovered = true;
        ApplyReply(window, root, hitWidget, hitWidget->OnPointerEnter());
    }
}

void UIInputRouter::ApplyReply(Window& window, Widget& root, Widget* target, const EventReply& reply)
{
    if (reply.capturePointer)
    {
        if (capturedTarget.window && (capturedTarget.window != &window || capturedTarget.widgetId != target->GetId()))
        {
            CancelCaptureForWindow(*capturedTarget.window);
        }

        capturedTarget = {
            .window = &window,
            .widgetId = target->GetId()
        };
        target->pointerCaptured = true;
    }

    if (reply.releasePointer && capturedTarget.window == &window)
    {
        if (Widget* captured = FindTargetWidget(capturedTarget))
        {
            captured->pointerCaptured = false;
        }

        capturedTarget = {};
    }

    if (reply.requestFocus && (focusedTarget.window != &window || focusedTarget.widgetId != target->GetId()))
    {
        if (Widget* previous = FindTargetWidget(focusedTarget))
        {
            previous->focused = false;
        }

        focusedTarget = {
            .window = &window,
            .widgetId = target->GetId()
        };
        target->focused = true;

        uiSystem.SetLastFocusedWidget(window, target->GetId());
    }

    if (reply.cursor.has_value())
    {
        window.ChangeCursor(*reply.cursor);
    }
}

bool UIInputRouter::TryClearInputState(Window& window, Widget& root, Widget& target, bool includeChildren)
{
    if (FindWidgetById(root, target.GetId()) != &target)
        return false;

    const auto isTarget = [&target, includeChildren](const Widget* widget)
    {
        if (!widget)
            return false;

        if (includeChildren)
            return IsInSubtree(widget, target);

        return widget == &target;
    };

    Widget* captured = capturedTarget.window == &window
                           ? FindWidgetById(root, capturedTarget.widgetId)
                           : nullptr;

    if (isTarget(captured))
        return false;

    const ObjectId lastFocusedId = uiSystem.GetLastFocusedWidget(window);
    Widget* lastFocused = FindWidgetById(root, lastFocusedId);

    if (isTarget(lastFocused))
    {
        uiSystem.SetLastFocusedWidget(window, {});
    }

    Widget* focused = FindWidgetById(root, focusedTarget.widgetId);
    if (isTarget(focused))
    {
        focused->focused = false;
        focusedTarget = {};
    }

    Widget* hovered = FindWidgetById(root, hoveredTarget.widgetId);
    if (hoveredTarget.window == &window && isTarget(hovered))
    {
        hovered->hovered = false;
        hoveredTarget = {};

        const EventReply reply = hovered->OnPointerLeave();

        if (reply.cursor.has_value())
            window.ChangeCursor(*reply.cursor);
    }

    return true;
}

Widget* UIInputRouter::FindWidgetById(Widget& root, ObjectId id) const
{
    if (id == ObjectId{})
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

Widget* UIInputRouter::FindTargetWidget(const WidgetInputTarget& target) const
{
    if (!target.window)
        return nullptr;

    Widget* root = uiSystem.FindWindowRoot(*target.window);
    return root ? FindWidgetById(*root, target.widgetId) : nullptr;
}

} // namespace URay
