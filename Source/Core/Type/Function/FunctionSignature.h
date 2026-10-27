#pragma once

#include <Core/Type/Function/FunctionParameter.h>

#include <optional>
#include <span>
#include <vector>

namespace URay
{

struct FunctionSignature
{
    std::vector<FunctionParameter> parameters;
    std::optional<FunctionParameter> returnValue;

    bool AcceptsArguments(std::span<const Variant> args) const;
};

} // namespace URay
