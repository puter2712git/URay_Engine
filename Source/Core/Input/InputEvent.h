#pragma once

#include "Core/Math/Vector2.h"

#include <cstdint>

namespace URay
{

enum class MouseButton : uint8_t
{
    None = 0,
    Left = 1 << 0,
    Right = 1 << 1,
    Middle = 1 << 2,
};

enum class ModifierKey : uint8_t
{
    None = 0,
    Shift = 1 << 0,
    Ctrl = 1 << 1,
    Alt = 1 << 2,
    Super = 1 << 3,
};

enum class KeyCode : uint16_t
{
    Unknown,
    A,
    B,
    C,
    Space,
    Escape,
    Enter,
    Tab,
    Left,
    Right,
    Up,
    Down,
};

struct PointerEvent
{
    uint32_t pointerId = 0; // Only mouse for now on.
    Vector2 position = Vector2::Zero;
    Vector2 delta = Vector2::Zero;

    MouseButton changedButton = MouseButton::Left;
    uint32_t pressedButtons = 0;

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
    bool isRepeat = false;
};

struct TextInputEvent
{
    char32_t codepoint = U'\0';
};

} // namespace URay
