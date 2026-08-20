#pragma once

#include "Core/Math/Vector2.h"

#include <cstdint>
#include <variant>

namespace URay
{

enum class MouseButton : uint8_t
{
    None = 0,
    Left = 1 << 0,
    Right = 1 << 1,
    Middle = 1 << 2,
};

using MouseButtonMask = uint8_t;

enum class ModifierKey : uint8_t
{
    None = 0,
    Shift = 1 << 0,
    Ctrl = 1 << 1,
    Alt = 1 << 2,
    Super = 1 << 3,
};

constexpr ModifierKey operator|(ModifierKey lhs, ModifierKey rhs)
{
    return static_cast<ModifierKey>(
        static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

constexpr ModifierKey operator&(ModifierKey lhs, ModifierKey rhs)
{
    return static_cast<ModifierKey>(
        static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

enum class KeyCode : uint16_t
{
    Unknown = 0,

    Space,
    Apostrophe,
    Comma,
    Minus,
    Period,
    Slash,

    Digit0,
    Digit1,
    Digit2,
    Digit3,
    Digit4,
    Digit5,
    Digit6,
    Digit7,
    Digit8,
    Digit9,

    Semicolon,
    Equal,

    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    LeftBracket,
    Backslash,
    RightBracket,
    GraveAccent,

    Escape,
    Enter,
    Tab,
    Backspace,
    Insert,
    Delete,

    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,

    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,

    Keypad0,
    Keypad1,
    Keypad2,
    Keypad3,
    Keypad4,
    Keypad5,
    Keypad6,
    Keypad7,
    Keypad8,
    Keypad9,
    KeypadDecimal,
    KeypadDivide,
    KeypadMultiply,
    KeypadSubtract,
    KeypadAdd,
    KeypadEnter,
    KeypadEqual,

    LeftShift,
    LeftControl,
    LeftAlt,
    LeftSuper,
    RightShift,
    RightControl,
    RightAlt,
    RightSuper,
    Menu,

    Count,
};

enum class KeyAction : uint8_t
{
    Pressed,
    Released,
    Repeated,
};

enum class PointerAction : uint8_t
{
    Moved,
    Pressed,
    Released,
};

struct PointerEvent
{
    PointerAction action = PointerAction::Moved;

    uint32_t pointerId = 0; // Only mouse for now on.
    Vector2 position = Vector2::Zero;
    Vector2 delta = Vector2::Zero;

    MouseButton changedButton = MouseButton::None;
    MouseButtonMask pressedButtons = 0;
    ModifierKey modifiers = ModifierKey::None;
};

struct ScrollEvent
{
    Vector2 position = Vector2::Zero;
    Vector2 delta = Vector2::Zero;

    ModifierKey modifiers = ModifierKey::None;
};

struct KeyEvent
{
    KeyCode key = KeyCode::Unknown;
    ModifierKey modifiers = ModifierKey::None;
    KeyAction action = KeyAction::Pressed;
};

struct TextInputEvent
{
    char32_t codepoint = U'\0';
};

using InputEvent = std::variant<KeyEvent, PointerEvent, ScrollEvent, TextInputEvent>;

} // namespace URay
