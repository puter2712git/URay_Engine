#pragma once

#include <Core/Type/Function/FunctionParameter.h>
#include <Core/Type/Variant/Variant.h>
#include <Engine/Object/Object.h>

#include <optional>
#include <type_traits>

namespace URay
{

template <typename T>
std::optional<std::remove_cv_t<T>> ReadCallableArgument(const Variant& argument)
{
    static_assert(!std::is_reference_v<T>, "Callable arguments must not be references.");

    using ArgumentType = std::remove_cv_t<T>;

    const auto parameter = MakeFunctionParameter<ArgumentType>();

    if (!parameter || !parameter->Accepts(argument))
        return std::nullopt;

    if constexpr (std::is_pointer_v<ArgumentType>)
    {
        using Pointee = std::remove_pointer_t<ArgumentType>;

        if constexpr (std::is_convertible_v<ArgumentType, Object*>)
        {
            ArgumentType object = argument.TryGetObject<Pointee>();

            if (!object && !argument.IsNullObject())
                return std::nullopt;

            return std::optional<ArgumentType>(object);
        }
        else
        {
            ArgumentType pointer = argument.TryGetPointer<Pointee>();

            if (!pointer && !argument.IsNullPointer())
                return std::nullopt;

            return std::optional<ArgumentType>(pointer);
        }
    }
    else
    {
        const ArgumentType* value = argument.TryGet<ArgumentType>();

        if (!value)
            return std::nullopt;

        return *value;
    }
}

} // namespace URay
