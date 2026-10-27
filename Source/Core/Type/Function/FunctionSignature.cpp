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

} // namespace URay
