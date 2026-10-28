#include "FunctionSignature.h"

#include <Core/Type/Variant/Variant.h>

namespace URay
{

bool FunctionSignature::AcceptsArguments(std::span<const Variant> args) const
{
    if (parameters.size() != args.size())
        return false;

    for (size_t index = 0; index < parameters.size(); ++index)
    {
        if (!parameters[index].Accepts(args[index]))
            return false;
    }

    return true;
}

bool FunctionSignature::Accepts(const FunctionSignature& source) const
{
    if (returnValue.has_value() || source.returnValue.has_value())
        return false;

    if (parameters.size() != source.parameters.size())
        return false;

    for (std::size_t index = 0; index < parameters.size(); ++index)
    {
        if (!parameters[index].Accepts(source.parameters[index]))
            return false;
    }

    return true;
}

} // namespace URay
