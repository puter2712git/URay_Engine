#include "InputActionMap.h"

#include <algorithm>
#include <utility>

namespace URay
{

bool InputActionMap::AddAction(InputActionDefinition definition)
{
    if (!definition.id.IsValid())
        return false;

    if (definition.bindings.empty())
        return false;

    if (FindAction(definition.id) != nullptr)
        return false;

    const bool hasIncompatibleBinding = std::any_of(
        definition.bindings.begin(),
        definition.bindings.end(),
        [&definition](const InputBinding& binding)
        {
            return !IsBindingCompatible(definition.valueType, binding);
        });

    if (hasIncompatibleBinding)
        return false;

    actions.push_back(std::move(definition));
    return true;
}

const InputActionDefinition* InputActionMap::FindAction(InputActionId id) const
{
    const auto iterator = std::find_if(
        actions.begin(),
        actions.end(),
        [id](const InputActionDefinition& definition)
        {
            return definition.id == id;
        });

    return iterator != actions.end() ? &(*iterator) : nullptr;
}

bool InputActionMap::IsBindingCompatible(InputActionValueType valueType, const InputBinding& binding) const
{
    switch (valueType)
    {
    case InputActionValueType::Button:
        return std::holds_alternative<KeyBinding>(binding) ||
               std::holds_alternative<MouseButtonBinding>(binding);

    case InputActionValueType::Axis1D:
        return std::holds_alternative<Axis1DCompositeBinding>(binding);

    case InputActionValueType::Axis2D:
        return std::holds_alternative<Axis2DCompositeBinding>(binding);
    }

    return false;
}

} // namespace URay
