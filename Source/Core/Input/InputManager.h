#pragma once

#include "Core/Input/InputEvent.h"

#include <vector>

namespace URay
{

class InputManager
{
public:
    void Update();
    void ClearEvents();

    void OnKey(KeyCode key, KeyAction action, ModifierKey modifiers);
    void OnMouseButton(MouseButton button, KeyAction action, ModifierKey modifiers);
    void OnCursorMoved(Vector2 position);

    bool GetKey(KeyCode key) const;
    bool GetKeyDown(KeyCode key) const;
    bool GetKeyUp(KeyCode key) const;

    bool GetMouse(MouseButton button) const;
    bool GetMouseDown(MouseButton button) const;
    bool GetMouseUp(MouseButton button) const;

    float GetMouseX() const { return static_cast<float>(mouseX); }
    float GetMouseY() const { return static_cast<float>(mouseY); }

    float GetMouseDeltaX() const { return static_cast<float>(mouseDeltaX); }
    float GetMouseDeltaY() const { return static_cast<float>(mouseDeltaY); }

    const std::vector<InputEvent>& GetEvents() const { return events; }

private:
    bool currKeys[static_cast<size_t>(KeyCode::Count)] = {};
    bool prevKeys[static_cast<size_t>(KeyCode::Count)] = {};

    MouseButtonMask currMouseButtons = 0;
    MouseButtonMask prevMouseButtons = 0;

    double mouseX = 0.0;
    double mouseY = 0.0;

    double prevMouseX = 0.0;
    double prevMouseY = 0.0;

    double mouseDeltaX = 0.0;
    double mouseDeltaY = 0.0;

    double scrollX = 0.0;
    double scrollY = 0.0;

    std::vector<InputEvent> events;
};

} // namespace URay
