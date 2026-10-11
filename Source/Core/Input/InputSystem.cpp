#include "InputSystem.h"

#include <Core/Log/LogSystem.h>

#include <cstring>

namespace URay
{

void InputSystem::Update()
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

void InputSystem::ClearEvents()
{
    events.clear();
}

void InputSystem::OnKey(KeyCode key, KeyAction action, ModifierKey modifiers)
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

void InputSystem::OnMouseButton(MouseButton button, KeyAction action, ModifierKey modifiers)
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

void InputSystem::OnCursorMoved(Vector2 position)
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
        .modifiers = GetCurrentModifiers() });
}

void InputSystem::OnTextInput(char32_t codepoint)
{
    events.push_back(TextInputEvent{
        .codepoint = codepoint });
}

void InputSystem::OnScroll(Vector2 delta)
{
    scrollX += delta.x;
    scrollY += delta.y;

    events.push_back(ScrollEvent{
        .position = Vector2(mouseX, mouseY),
        .delta = delta,
        .modifiers = GetCurrentModifiers() });
}

bool InputSystem::GetKey(KeyCode key) const
{
    const size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= static_cast<size_t>(KeyCode::Count))
        return false;

    return currKeys[keyIndex];
}

bool InputSystem::GetKeyDown(KeyCode key) const
{
    const size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= static_cast<size_t>(KeyCode::Count))
        return false;

    return currKeys[keyIndex] && !prevKeys[keyIndex];
}

bool InputSystem::GetKeyUp(KeyCode key) const
{
    const size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= static_cast<size_t>(KeyCode::Count))
        return false;

    return !currKeys[keyIndex] && prevKeys[keyIndex];
}

bool InputSystem::GetMouse(MouseButton button) const
{
    const MouseButtonMask mask = static_cast<MouseButtonMask>(button);
    return button != MouseButton::None && (currMouseButtons & mask) != 0;
}

bool InputSystem::GetMouseDown(MouseButton button) const
{
    const MouseButtonMask mask = static_cast<MouseButtonMask>(button);
    return button != MouseButton::None && (currMouseButtons & mask) != 0 && (prevMouseButtons & mask) == 0;
}

bool InputSystem::GetMouseUp(MouseButton button) const
{
    const MouseButtonMask mask = static_cast<MouseButtonMask>(button);
    return button != MouseButton::None && (currMouseButtons & mask) == 0 && (prevMouseButtons & mask) != 0;
}

ModifierKey InputSystem::GetCurrentModifiers() const
{
    ModifierKey modifiers = ModifierKey::None;

    if (GetKey(KeyCode::LeftShift) || GetKey(KeyCode::RightShift))
        modifiers |= ModifierKey::Shift;

    if (GetKey(KeyCode::LeftControl) || GetKey(KeyCode::RightControl))
        modifiers |= ModifierKey::Ctrl;

    if (GetKey(KeyCode::LeftAlt) || GetKey(KeyCode::RightAlt))
        modifiers |= ModifierKey::Alt;

    if (GetKey(KeyCode::LeftSuper) || GetKey(KeyCode::RightSuper))
        modifiers |= ModifierKey::Super;

    return modifiers;
}

} // namespace URay
