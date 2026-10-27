#pragma once

#include <Core/Type/Function/FunctionParameter.h>

#include <optional>
#include <vector>

namespace URay
{

struct FunctionSignature
{
    std::vector<FunctionParameter> parameters;
    std::optional<FunctionParameter> returnValue;
};

} // namespace URay
