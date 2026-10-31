#pragma once

#include <Core/Input/InputEvent.h>
#include <Core/Math/Vector2.h>
#include <Engine/UI/Widget/Widget.h>

namespace URay
{

class UISystem;
class Window;
struct EventReply;
struct InputSnapshot;

class UIInputRouter
{
    using PointerEventHandler = EventReply (Widget::*)(const PointerEvent&);

    struct WidgetInputTarget
    {
        Window* window = nullptr;
        ObjectId widgetId = {};
    };

public:
    UIInputRouter(UISystem& uiSystem);

public:
    void Process(Window& window, Widget& root, const InputSnapshot& input);
    void ProcessEvent(Window& window, Widget& root, const InputEvent& event);

    bool IsTextInputActive(Widget& root) const;

    bool TryClearInputStateForWidget(Window& window, Widget& root, Widget& widget);
    bool TryClearInputStateForSubtree(Window& window, Widget& root, Widget& subtree);

    void ClearHoverForWindow(Window& window);
    void ClearFocusForWindow(Window& window);
    void CancelCaptureForWindow(Window& window);

    void RestoreFocusForWindow(Window& window);

    Widget* FindTopWidget(Widget& root, const Vector2& position) const;

private:
    void DispatchKeyEvent(Widget& root, const KeyEvent& event);
    void DispatchPointerEvent(Window& window, Widget& root, const PointerEvent& event);
    void DispatchScrollEvent(Window& window, Widget& root, const ScrollEvent& event);
    void DispatchTextInputEvent(Window& window, Widget& root, const TextInputEvent& event);

    void BubblePointerEvent(Window& window, Widget& root, Widget* target, const PointerEvent& event, PointerEventHandler handler);

    void UpdateHoveredWidget(Window& window, Widget& root, Widget* hitWidget);
    void ApplyReply(Window& window, Widget& root, Widget* target, const EventReply& reply);

    bool TryClearInputState(Window& window, Widget& root, Widget& target, bool includeChildren);

    Widget* FindWidgetById(Widget& root, ObjectId id) const;
    Widget* FindTargetWidget(const WidgetInputTarget& target) const;

private:
    UISystem& uiSystem;

    WidgetInputTarget hoveredTarget = {};
    WidgetInputTarget focusedTarget = {};
    WidgetInputTarget capturedTarget = {};
};

} // namespace URay
