#include "InputActionEvaluator.h"

#include <Core/Math/Math.h>

#include <Core/Input/InputSnapshot.h>

#include <algorithm>
#include <type_traits>

namespace URay
{

namespace
{

bool AreRequiredModifiersHeld(
    const InputSnapshot& input,
    ModifierKey requiredModifiers)
{
    const ModifierKey heldModifiers = input.GetHeldModifiers();

    return (heldModifiers & requiredModifiers) == requiredModifiers;
}

float EvaluateAxis1D(
    const InputSnapshot& input,
    const Axis1DCompositeBinding& binding)
{
    const float negative =
        input.IsKeyHeld(binding.negative) ? 1.0f : 0.0f;

    const float positive =
        input.IsKeyHeld(binding.positive) ? 1.0f : 0.0f;

    return positive - negative;
}

Vector2 EvaluateAxis2D(
    const InputSnapshot& input,
    const Axis2DCompositeBinding& binding)
{
    Vector2 value = Vector2::Zero;

    value.x =
        (input.IsKeyHeld(binding.positiveX) ? 1.0f : 0.0f) -
        (input.IsKeyHeld(binding.negativeX) ? 1.0f : 0.0f);

    value.y =
        (input.IsKeyHeld(binding.positiveY) ? 1.0f : 0.0f) -
        (input.IsKeyHeld(binding.negativeY) ? 1.0f : 0.0f);

    return value;
}

const InputActionValue* FindPreviousAction(
    const std::vector<InputActionState>& actions,
    InputActionId id)
{
    const auto iterator = std::find_if(
        actions.begin(),
        actions.end(),
        [id](const InputActionState& state)
        {
            return state.id == id;
        });

    return iterator != actions.end() ? &iterator->value : nullptr;
}

} // namespace

InputActionSnapshot InputActionEvaluator::Evaluate(
    const InputSnapshot& input,
    const InputActionMap& actionMap)
{
    InputActionSnapshot result;
    result.frameIndex = input.GetFrameIndex();

    const std::vector<InputActionDefinition>& definitions = actionMap.GetActions();

    result.actions.reserve(definitions.size());

    for (const InputActionDefinition& definition : definitions)
    {
        InputActionValue value = {};

        if (input.IsWindowFocused())
        {
            for (const InputBinding& inputBinding : definition.bindings)
            {
                std::visit(
                    [&input, &value](const auto& binding)
                    {
                        using BindingType = std::decay_t<decltype(binding)>;

                        if constexpr (std::is_same_v<BindingType, KeyBinding>)
                        {
                            const bool held = input.IsKeyHeld(binding.key) &&
                                              AreRequiredModifiersHeld(input, binding.requiredModifiers);

                            value.held = value.held || held;
                        }
                        else if constexpr (std::is_same_v<BindingType, MouseButtonBinding>)
                        {
                            const bool held = input.IsMouseButtonHeld(binding.button) &&
                                              AreRequiredModifiersHeld(input, binding.requiredModifiers);

                            value.held = value.held || held;
                        }
                        else if constexpr (std::is_same_v<BindingType, Axis1DCompositeBinding>)
                        {
                            value.axis1D += EvaluateAxis1D(input, binding);
                        }
                        else if constexpr (std::is_same_v<BindingType, Axis2DCompositeBinding>)
                        {
                            value.axis2D += EvaluateAxis2D(input, binding);
                        }
                    },
                    inputBinding);
            }
        }

        switch (definition.valueType)
        {
        case InputActionValueType::Button:
            break;

        case InputActionValueType::Axis1D:
            value.axis1D = Math::Clamp(value.axis1D, -1.0f, 1.0f);
            value.held = value.axis1D != 0.0f;
            break;

        case InputActionValueType::Axis2D:
            value.axis2D.x = Math::Clamp(value.axis2D.x, -1.0f, 1.0f);
            value.axis2D.y = Math::Clamp(value.axis2D.y, -1.0f, 1.0f);
            value.held =
                value.axis2D.x != 0.0f ||
                value.axis2D.y != 0.0f;
            break;
        }

        const InputActionValue* previousValue =
            FindPreviousAction(previousActions, definition.id);

        const bool wasHeld =
            previousValue != nullptr && previousValue->held;

        value.pressed = !wasHeld && value.held;
        value.released = wasHeld && !value.held;

        result.actions.push_back({
            .id = definition.id,
            .value = value,
        });
    }

    previousActions = result.actions;
    return result;
}

} // namespace URay
