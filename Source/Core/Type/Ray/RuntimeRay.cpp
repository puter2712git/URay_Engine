#include "RuntimeRay.h"

namespace URay
{

RuntimeRay::RuntimeRay(FunctionSignature signature) : signature(std::move(signature)) {}

bool RuntimeRay::Register(std::unique_ptr<Callable> callback)
{
    if (!callback)
        return false;

    if (!callback->GetSignature().Accepts(signature))
        return false;

    callbacks.push_back(std::move(callback));
    return true;
}

bool RuntimeRay::Emit(std::span<const Variant> args)
{
    if (!signature.AcceptsArguments(args))
        return false;

    bool allInvoked = true;
    const std::size_t count = callbacks.size();

    for (std::size_t index = 0; index < count; ++index)
    {
        if (!callbacks[index]->Invoke(args))
            allInvoked = false;
    }

    return allInvoked;
}

} // namespace URay
