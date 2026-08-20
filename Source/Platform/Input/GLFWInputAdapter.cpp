#include "GLFWInputAdapter.h"

#include <GLFW/glfw3.h>

namespace URay::Platform
{

namespace
{

static_assert(static_cast<uint16_t>(KeyCode::Digit9) == static_cast<uint16_t>(KeyCode::Digit0) + 9);
static_assert(static_cast<uint16_t>(KeyCode::Z) == static_cast<uint16_t>(KeyCode::A) + 25);
static_assert(static_cast<uint16_t>(KeyCode::F25) == static_cast<uint16_t>(KeyCode::F1) + 24);
static_assert(static_cast<uint16_t>(KeyCode::Keypad9) == static_cast<uint16_t>(KeyCode::Keypad0) + 9);

constexpr KeyCode ToContiguousKeyCode(int key, int firstNativeKey, KeyCode firstEngineKey, int count)
{
    if (key < firstNativeKey || key >= firstNativeKey + count)
        return KeyCode::Unknown;

    return static_cast<KeyCode>(
        static_cast<uint16_t>(firstEngineKey) + static_cast<uint16_t>(key - firstNativeKey));
}

} // namespace

KeyCode ToKeyCode(int glfwKey)
{
    if (const KeyCode key = ToContiguousKeyCode(glfwKey, GLFW_KEY_0, KeyCode::Digit0, 10); key != KeyCode::Unknown)
        return key;

    if (const KeyCode key = ToContiguousKeyCode(glfwKey, GLFW_KEY_A, KeyCode::A, 26); key != KeyCode::Unknown)
        return key;

    if (const KeyCode key = ToContiguousKeyCode(glfwKey, GLFW_KEY_F1, KeyCode::F1, 25); key != KeyCode::Unknown)
        return key;

    if (const KeyCode key = ToContiguousKeyCode(glfwKey, GLFW_KEY_KP_0, KeyCode::Keypad0, 10); key != KeyCode::Unknown)
        return key;

    switch (glfwKey)
    {
    case GLFW_KEY_SPACE: return KeyCode::Space;
    case GLFW_KEY_APOSTROPHE: return KeyCode::Apostrophe;
    case GLFW_KEY_COMMA: return KeyCode::Comma;
    case GLFW_KEY_MINUS: return KeyCode::Minus;
    case GLFW_KEY_PERIOD: return KeyCode::Period;
    case GLFW_KEY_SLASH: return KeyCode::Slash;
    case GLFW_KEY_SEMICOLON: return KeyCode::Semicolon;
    case GLFW_KEY_EQUAL: return KeyCode::Equal;
    case GLFW_KEY_LEFT_BRACKET: return KeyCode::LeftBracket;
    case GLFW_KEY_BACKSLASH: return KeyCode::Backslash;
    case GLFW_KEY_RIGHT_BRACKET: return KeyCode::RightBracket;
    case GLFW_KEY_GRAVE_ACCENT: return KeyCode::GraveAccent;
    case GLFW_KEY_ESCAPE: return KeyCode::Escape;
    case GLFW_KEY_ENTER: return KeyCode::Enter;
    case GLFW_KEY_TAB: return KeyCode::Tab;
    case GLFW_KEY_BACKSPACE: return KeyCode::Backspace;
    case GLFW_KEY_INSERT: return KeyCode::Insert;
    case GLFW_KEY_DELETE: return KeyCode::Delete;
    case GLFW_KEY_RIGHT: return KeyCode::Right;
    case GLFW_KEY_LEFT: return KeyCode::Left;
    case GLFW_KEY_DOWN: return KeyCode::Down;
    case GLFW_KEY_UP: return KeyCode::Up;
    case GLFW_KEY_PAGE_UP: return KeyCode::PageUp;
    case GLFW_KEY_PAGE_DOWN: return KeyCode::PageDown;
    case GLFW_KEY_HOME: return KeyCode::Home;
    case GLFW_KEY_END: return KeyCode::End;
    case GLFW_KEY_CAPS_LOCK: return KeyCode::CapsLock;
    case GLFW_KEY_SCROLL_LOCK: return KeyCode::ScrollLock;
    case GLFW_KEY_NUM_LOCK: return KeyCode::NumLock;
    case GLFW_KEY_PRINT_SCREEN: return KeyCode::PrintScreen;
    case GLFW_KEY_PAUSE: return KeyCode::Pause;
    case GLFW_KEY_KP_DECIMAL: return KeyCode::KeypadDecimal;
    case GLFW_KEY_KP_DIVIDE: return KeyCode::KeypadDivide;
    case GLFW_KEY_KP_MULTIPLY: return KeyCode::KeypadMultiply;
    case GLFW_KEY_KP_SUBTRACT: return KeyCode::KeypadSubtract;
    case GLFW_KEY_KP_ADD: return KeyCode::KeypadAdd;
    case GLFW_KEY_KP_ENTER: return KeyCode::KeypadEnter;
    case GLFW_KEY_KP_EQUAL: return KeyCode::KeypadEqual;
    case GLFW_KEY_LEFT_SHIFT: return KeyCode::LeftShift;
    case GLFW_KEY_LEFT_CONTROL: return KeyCode::LeftControl;
    case GLFW_KEY_LEFT_ALT: return KeyCode::LeftAlt;
    case GLFW_KEY_LEFT_SUPER: return KeyCode::LeftSuper;
    case GLFW_KEY_RIGHT_SHIFT: return KeyCode::RightShift;
    case GLFW_KEY_RIGHT_CONTROL: return KeyCode::RightControl;
    case GLFW_KEY_RIGHT_ALT: return KeyCode::RightAlt;
    case GLFW_KEY_RIGHT_SUPER: return KeyCode::RightSuper;
    case GLFW_KEY_MENU: return KeyCode::Menu;
    default: return KeyCode::Unknown;
    }
}

MouseButton ToMouseButton(int glfwButton)
{
    switch (glfwButton)
    {
    case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::Left;
    case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::Right;
    case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
    default: return MouseButton::None;
    }
}

ModifierKey ToModifierKey(int glfwModifiers)
{
    ModifierKey modifiers = ModifierKey::None;

    if (glfwModifiers & GLFW_MOD_SHIFT)
        modifiers = modifiers | ModifierKey::Shift;
    if (glfwModifiers & GLFW_MOD_CONTROL)
        modifiers = modifiers | ModifierKey::Ctrl;
    if (glfwModifiers & GLFW_MOD_ALT)
        modifiers = modifiers | ModifierKey::Alt;
    if (glfwModifiers & GLFW_MOD_SUPER)
        modifiers = modifiers | ModifierKey::Super;

    return modifiers;
}

std::optional<KeyAction> ToKeyAction(int glfwAction)
{
    switch (glfwAction)
    {
    case GLFW_PRESS: return KeyAction::Pressed;
    case GLFW_RELEASE: return KeyAction::Released;
    case GLFW_REPEAT: return KeyAction::Repeated;
    default: return std::nullopt;
    }
}

} // namespace URay::Platform
