#pragma once

#include "Core/Input/InputEvent.h"
#include "Core/Math/Vector2.h"

namespace URay
{

class Widget;
class InputManager;

class UIInputRouter
{
public:
    void Process(Widget& root, const InputManager& input);

private:
    Widget* FindTopWidget(Widget& root, const Vector2& position) const;

    PointerEvent MakePointerEvent(const InputManager& input, MouseButton mouseButton) const;

private:
    Widget* hoveredWidget = nullptr;
    Widget* focusedWidget = nullptr;
    Widget* pointerCapturedWidget = nullptr;
};

} // namespace URay
