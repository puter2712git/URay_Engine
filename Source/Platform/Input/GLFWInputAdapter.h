#pragma once

#include "Core/Input/InputEvent.h"

#include <optional>

namespace URay::Platform
{

// Translates GLFW's native input values at the platform boundary.
KeyCode ToKeyCode(int glfwKey);
MouseButton ToMouseButton(int glfwButton);
ModifierKey ToModifierKey(int glfwModifiers);
std::optional<KeyAction> ToKeyAction(int glfwAction);

} // namespace URay::Platform
