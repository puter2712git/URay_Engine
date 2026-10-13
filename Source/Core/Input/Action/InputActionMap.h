#pragma once

#include <Core/Input/Action/InputAction.h>
#include <Core/Input/InputEvent.h>

#include <variant>
#include <vector>

namespace URay
{

struct KeyBinding
{
    KeyCode key = KeyCode::Unknown;
    ModifierKey requiredModifiers = ModifierKey::None;
};

struct MouseButtonBinding
{
    MouseButton button = MouseButton::None;
    ModifierKey requiredModifiers = ModifierKey::None;
};

struct Axis1DCompositeBinding
{
    KeyCode negative = KeyCode::Unknown;
    KeyCode positive = KeyCode::Unknown;
};

struct Axis2DCompositeBinding
{
    KeyCode negativeX = KeyCode::Unknown;
    KeyCode positiveX = KeyCode::Unknown;
    KeyCode negativeY = KeyCode::Unknown;
    KeyCode positiveY = KeyCode::Unknown;
};

using InputBinding = std::variant<
    KeyBinding,
    MouseButtonBinding,
    Axis1DCompositeBinding,
    Axis2DCompositeBinding>;

struct InputActionDefinition
{
    InputActionId id = {};
    InputActionValueType valueType = InputActionValueType::Button;

    std::vector<InputBinding> bindings;
};

class InputActionMap
{
public:
    bool AddAction(InputActionDefinition definition);
    const InputActionDefinition* FindAction(InputActionId id) const;
    const std::vector<InputActionDefinition>& GetActions() const { return actions; }

private:
    std::vector<InputActionDefinition> actions;
};

} // namespace URay
