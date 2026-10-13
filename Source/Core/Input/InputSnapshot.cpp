#include "InputSnapshot.h"

#include <iterator>
#include <optional>

namespace URay
{

std::optional<size_t> InputSnapshot::GetMouseButtonIndex(MouseButton button)
{
    switch (button)
    {
    case MouseButton::Left: return 0;
    case MouseButton::Right: return 1;
    case MouseButton::Middle: return 2;
    default: return std::nullopt;
    }
}

MouseButton InputSnapshot::GetMouseButtonByIndex(size_t index)
{
    constexpr MouseButton buttons[] = {
        MouseButton::Left,
        MouseButton::Right,
        MouseButton::Middle,
    };

    return index < std::size(buttons)
               ? buttons[index]
               : MouseButton::None;
}

const InputButtonState* InputSnapshot::GetKeyState(KeyCode key) const
{
    const size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= keys.size())
        return nullptr;

    return &keys[keyIndex];
}

const InputButtonState* InputSnapshot::GetMouseButtonState(MouseButton button) const
{
    const std::optional<size_t> buttonIndex = GetMouseButtonIndex(button);
    if (!buttonIndex)
        return nullptr;

    return &mouseButtons[*buttonIndex];
}

bool InputSnapshot::IsKeyHeld(KeyCode key) const
{
    const InputButtonState* state = GetKeyState(key);
    return state && state->held;
}

bool InputSnapshot::WasKeyPressed(KeyCode key) const
{
    const InputButtonState* state = GetKeyState(key);
    return state && state->pressed;
}

bool InputSnapshot::WasKeyReleased(KeyCode key) const
{
    const InputButtonState* state = GetKeyState(key);
    return state && state->released;
}

bool InputSnapshot::IsMouseButtonHeld(MouseButton button) const
{
    const InputButtonState* state = GetMouseButtonState(button);
    return state && state->held;
}

bool InputSnapshot::WasMouseButtonPressed(MouseButton button) const
{
    const InputButtonState* state = GetMouseButtonState(button);
    return state && state->pressed;
}

bool InputSnapshot::WasMouseButtonReleased(MouseButton button) const
{
    const InputButtonState* state = GetMouseButtonState(button);
    return state && state->released;
}

ModifierKey InputSnapshot::GetHeldModifiers() const
{
    ModifierKey modifiers = ModifierKey::None;

    if (IsKeyHeld(KeyCode::LeftShift) || IsKeyHeld(KeyCode::RightShift))
        modifiers |= ModifierKey::Shift;

    if (IsKeyHeld(KeyCode::LeftControl) || IsKeyHeld(KeyCode::RightControl))
        modifiers |= ModifierKey::Ctrl;

    if (IsKeyHeld(KeyCode::LeftAlt) || IsKeyHeld(KeyCode::RightAlt))
        modifiers |= ModifierKey::Alt;

    if (IsKeyHeld(KeyCode::LeftSuper) || IsKeyHeld(KeyCode::RightSuper))
        modifiers |= ModifierKey::Super;

    return modifiers;
}

} // namespace URay
