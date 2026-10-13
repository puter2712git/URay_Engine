#pragma once

#include <Core/Input/InputEvent.h>
#include <Core/Math/Vector2.h>
#include <Editor/Widget/Widget.h>

namespace URay
{

class Window;

struct EventReply;
struct InputSnapshot;

class UIInputRouter
{
    using PointerEventHandler = EventReply (Widget::*)(const PointerEvent&);

public:
    UIInputRouter(Window& window);

public:
    void Process(Widget& root, const InputSnapshot& input);
    bool IsTextInputActive(Widget& root) const;

private:
    void DispatchKeyEvent(Widget& root, const KeyEvent& event);
    void DispatchPointerEvent(Widget& root, const PointerEvent& event);
    void DispatchScrollEvent(Widget& root, const ScrollEvent& event);
    void DispatchTextInputEvent(Widget& root, const TextInputEvent& event);

    void BubblePointerEvent(Widget& root, Widget* target, const PointerEvent& event, PointerEventHandler handler);

    void UpdateHoveredWidget(Widget& root, Widget* hitWidget);
    void ApplyReply(Widget& root, Widget* target, const EventReply& reply);

    Widget* FindTopWidget(Widget& root, const Vector2& position) const;
    Widget* FindWidgetById(Widget& root, WidgetId id) const;

private:
    Window& window;

    WidgetId hoveredWidgetId = InvalidWidgetId;
    WidgetId focusedWidgetId = InvalidWidgetId;
    WidgetId pointerCapturedWidgetId = InvalidWidgetId;
};

} // namespace URay
