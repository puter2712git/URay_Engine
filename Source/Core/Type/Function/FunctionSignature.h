#pragma once

#include <Core/Type/Function/FunctionParameter.h>
#include <Core/Type/Function/FunctionTraits.h>

#include <optional>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace URay
{

struct FunctionSignature
{
    std::vector<FunctionParameter> parameters;
    std::optional<FunctionParameter> returnValue;

    bool AcceptsArguments(std::span<const Variant> args) const;
    bool Accepts(const FunctionSignature& source) const;
};

namespace Detail
{

template <typename Traits, std::size_t... Indices>
std::optional<FunctionSignature> MakeFunctionSignatureImpl(std::index_sequence<Indices...>)
{
    using ArgumentTypes = typename Traits::ArgumentTypes;
    using ReturnType = typename Traits::ReturnType;

    FunctionSignature signature;

    auto addParameter = [&signature]<typename T>() -> bool
    {
        auto parameter = MakeFunctionParameter<T>();

        if (!parameter)
            return false;

        signature.parameters.push_back(*parameter);
        return true;
    };

    const bool parametersValid =
        (addParameter.template operator()<std::tuple_element_t<Indices, ArgumentTypes>>() && ...);

    if (!parametersValid)
        return std::nullopt;

    if constexpr (!std::is_void_v<ReturnType>)
    {
        auto returnParameter = MakeFunctionParameter<ReturnType>();

        if (!returnParameter)
            return std::nullopt;

        signature.returnValue = *returnParameter;
    }

    return signature;
}

} // namespace Detail

template <typename FunctionPointer>
std::optional<FunctionSignature> MakeFunctionSignature(FunctionPointer)
{
    using Traits = FunctionTraits<FunctionPointer>;

    return Detail::MakeFunctionSignatureImpl<Traits>(
        std::make_index_sequence<Traits::ArgumentCount>{});
}

} // namespace URay
