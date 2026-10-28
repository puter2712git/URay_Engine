#pragma once

#include <Core/Type/Function/Callable/Callable.h>
#include <Core/Type/Function/Callable/CallableArgument.h>
#include <Core/Type/Function/FunctionTraits.h>
#include <Core/Type/Variant/Variant.h>

#include <functional>
#include <memory>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace URay
{

template <typename Method>
class MemberCallable : public Callable
{
private:
    using Traits = FunctionTraits<Method>;
    using Owner = typename Traits::OwnerType;
    using ArgumentTypes = typename Traits::ArgumentTypes;

    static_assert(Traits::IsMember, "Method must be a member function.");
    static_assert(
        std::is_void_v<typename Traits::ReturnType>,
        "Ray callbacks must return void.");

public:
    MemberCallable(Owner* target, Method method, FunctionSignature signature)
        : Callable(std::move(signature)), method(method)
    {
        if constexpr (std::is_convertible_v<Owner*, Object*>)
        {
            objectTarget.emplace(static_cast<Object*>(target));
        }
        else
        {
            plainTarget = target;
        }
    }

    bool Invoke(std::span<const Variant> args) override
    {
        Owner* target = ResolveTarget();

        if (!target)
            return false;

        if (!GetSignature().AcceptsArguments(args))
            return false;

        return InvokeImpl(
            target,
            args,
            std::make_index_sequence<Traits::ArgumentCount>{});
    }

private:
    Owner* ResolveTarget() const
    {
        if constexpr (std::is_convertible_v<Owner*, Object*>)
        {
            return objectTarget->TryGetObject<Owner>();
        }
        else
        {
            return plainTarget;
        }
    }

    template <std::size_t... Indices>
    bool InvokeImpl(Owner* target, std::span<const Variant> args, std::index_sequence<Indices...>)
    {
        auto values = std::tuple{
            ReadCallableArgument<
                std::tuple_element_t<Indices, ArgumentTypes>>(args[Indices])...
        };

        const bool allValid = std::apply(
            [](const auto&... value)
            {
                return (value.has_value() && ...);
            },
            values);

        if (!allValid)
            return false;

        std::invoke(method, target, *std::get<Indices>(values)...);
        return true;
    }

private:
    Method method;
    Owner* plainTarget = nullptr;
    std::optional<Variant> objectTarget;
};

template <typename Target, typename Method>
std::unique_ptr<Callable> MakeMemberCallable(Target* target, Method method)
{
    using Traits = FunctionTraits<Method>;
    using Owner = typename Traits::OwnerType;

    static_assert(Traits::IsMember, "Method must be a member function.");
    static_assert(
        std::is_convertible_v<Target*, Owner*>,
        "Target must be compatible with the method's owner type.");
    static_assert(
        std::is_void_v<typename Traits::ReturnType>,
        "Ray callbacks must return void.");

    if (!target || !method)
        return nullptr;

    auto signature = MakeFunctionSignature(method);

    if (!signature)
        return nullptr;

    return std::make_unique<MemberCallable<Method>>(
        static_cast<Owner*>(target),
        method,
        std::move(*signature));
}

} // namespace URay
