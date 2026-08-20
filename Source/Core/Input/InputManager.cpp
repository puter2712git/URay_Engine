#include "InputManager.h"

#include <cstring>

namespace URay
{

void InputManager::Update()
{
    std::memcpy(prevKeys, currKeys, sizeof(currKeys));
    prevMouseButtons = currMouseButtons;

    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;

    prevMouseX = mouseX;
    prevMouseY = mouseY;

    scrollX = 0.0;
    scrollY = 0.0;
}

void InputManager::ClearEvents()
{
    events.clear();
}

void InputManager::OnKey(KeyCode key, KeyAction action, ModifierKey modifiers)
{
    const size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= static_cast<size_t>(KeyCode::Count))
        return;

    currKeys[keyIndex] = action != KeyAction::Released;

    events.push_back(KeyEvent{
        .key = key,
        .modifiers = modifiers,
        .action = action,
    });
}

void InputManager::OnMouseButton(MouseButton button, KeyAction action, ModifierKey modifiers)
{
    const MouseButtonMask buttonMask = static_cast<MouseButtonMask>(button);

    if (action == KeyAction::Released)
    {
        currMouseButtons &= static_cast<MouseButtonMask>(~buttonMask);
    }
    else
    {
        currMouseButtons |= buttonMask;
    }

    events.push_back(PointerEvent{
        .action = action == KeyAction::Released
                      ? PointerAction::Released
                      : PointerAction::Pressed,
        .position = Vector2(mouseX, mouseY),
        .delta = Vector2::Zero,
        .changedButton = button,
        .pressedButtons = currMouseButtons,
        .modifiers = modifiers,
    });
}

void InputManager::OnCursorMoved(Vector2 position)
{
    const Vector2 delta = Vector2(position.x - mouseX, position.y - mouseY);

    mouseDeltaX += delta.x;
    mouseDeltaY += delta.y;

    mouseX = position.x;
    mouseY = position.y;

    events.push_back(PointerEvent{
        .action = PointerAction::Moved,
        .position = position,
        .delta = delta,
        .changedButton = MouseButton::None,
        .pressedButtons = currMouseButtons,
    });
}

bool InputManager::GetKey(KeyCode key) const
{
    const size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= static_cast<size_t>(KeyCode::Count))
        return false;

    return currKeys[keyIndex];
}

bool InputManager::GetKeyDown(KeyCode key) const
{
    const size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= static_cast<size_t>(KeyCode::Count))
        return false;

    return currKeys[keyIndex] && !prevKeys[keyIndex];
}

bool InputManager::GetKeyUp(KeyCode key) const
{
    const size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= static_cast<size_t>(KeyCode::Count))
        return false;

    return !currKeys[keyIndex] && prevKeys[keyIndex];
}

bool InputManager::GetMouse(MouseButton button) const
{
    const MouseButtonMask mask = static_cast<MouseButtonMask>(button);
    return button != MouseButton::None && (currMouseButtons & mask) != 0;
}

bool InputManager::GetMouseDown(MouseButton button) const
{
    const MouseButtonMask mask = static_cast<MouseButtonMask>(button);
    return button != MouseButton::None && (currMouseButtons & mask) != 0 && (prevMouseButtons & mask) == 0;
}

bool InputManager::GetMouseUp(MouseButton button) const
{
    const MouseButtonMask mask = static_cast<MouseButtonMask>(button);
    return button != MouseButton::None && (currMouseButtons & mask) == 0 && (prevMouseButtons & mask) != 0;
}

} // namespace URay
