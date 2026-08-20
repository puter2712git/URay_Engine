#pragma once

#include "Core/Input/InputEvent.h"
#include "Core/Math/Vector2.h"

namespace URay
{

class Widget;
class InputManager;

struct EventReply;

class UIInputRouter
{
public:
    void Process(Widget& root, const InputManager& input);

private:
    void DispatchKeyEvent(const KeyEvent& event);
    void DispatchPointerEvent(Widget& root, const PointerEvent& event);
    void DispatchScrollEvent(Widget& root, const ScrollEvent& event);
    void DispatchTextInputEvent(const TextInputEvent& event);

    void UpdateHoveredWidget(Widget* hitWidget);
    void ApplyReply(Widget* target, const EventReply& reply);

    Widget* FindTopWidget(Widget& root, const Vector2& position) const;

private:
    Widget* hoveredWidget = nullptr;
    Widget* focusedWidget = nullptr;
    Widget* pointerCapturedWidget = nullptr;
};

} // namespace URay
