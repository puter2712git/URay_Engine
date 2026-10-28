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
#include <vector>

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
    MemberCallable(Owner* target, const void* targetIdentity, Method method, FunctionSignature signature, std::vector<Variant> boundArguments)
        : Callable(std::move(signature)), method(method), targetIdentity(targetIdentity), boundArguments(std::move(boundArguments))
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

        if (args.size() + boundArguments.size() != Traits::ArgumentCount)
            return false;

        return InvokeImpl(
            target,
            args,
            std::make_index_sequence<Traits::ArgumentCount>{});
    }

    bool HasTarget(const void* target) const override { return targetIdentity == target; }

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
                std::tuple_element_t<Indices, ArgumentTypes>>(
                Indices < args.size()
                    ? args[Indices]
                    : boundArguments[Indices - args.size()])...
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
    const void* targetIdentity = nullptr;

    std::vector<Variant> boundArguments;

    Owner* plainTarget = nullptr;
    std::optional<Variant> objectTarget;
};

template <typename Target, typename Method, typename... BoundArgs>
std::unique_ptr<Callable> MakeMemberCallable(Target* target, Method method, BoundArgs&&... boundArgs)
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

    constexpr std::size_t boundCount = sizeof...(BoundArgs);

    if (boundCount > signature->parameters.size())
        return nullptr;

    std::vector<Variant> boundArguments;
    boundArguments.reserve(boundCount);
    (boundArguments.emplace_back(std::forward<BoundArgs>(boundArgs)), ...);

    const std::size_t eventArgumentCount = signature->parameters.size() - boundCount;

    for (std::size_t index = 0; index < boundCount; ++index)
    {
        const auto& parameter = signature->parameters[eventArgumentCount + index];

        if (!parameter.Accepts(boundArguments[index]))
            return nullptr;
    }

    signature->parameters.resize(eventArgumentCount);

    return std::make_unique<MemberCallable<Method>>(
        static_cast<Owner*>(target),
        static_cast<const void*>(target),
        method,
        std::move(*signature),
        std::move(boundArguments));
}

} // namespace URay
