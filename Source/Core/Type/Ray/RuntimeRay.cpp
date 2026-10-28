#include "RuntimeRay.h"

#include <algorithm>

namespace URay
{

RuntimeRay::RuntimeRay(FunctionSignature signature) : signature(std::move(signature)) {}

bool RuntimeRay::Register(std::unique_ptr<Callable> callback)
{
    if (!callback)
        return false;

    if (!callback->GetSignature().Accepts(signature))
        return false;

    callbacks.push_back(std::make_shared<Connection>(std::move(callback)));
    return true;
}

void RuntimeRay::UnregisterAll(const void* target)
{
    if (!target)
        return;

    for (const auto& connection : callbacks)
    {
        if (connection->callback->HasTarget(target))
            connection->active = false;
    }

    std::erase_if(
        callbacks,
        [](const auto& connection)
        {
            return !connection->active;
        });
}

bool RuntimeRay::Emit(std::span<const Variant> args)
{
    if (!signature.AcceptsArguments(args))
        return false;

    const auto currentConnections = callbacks;

    bool allInvoked = true;

    for (const auto& connection : currentConnections)
    {
        if (!connection->active)
            continue;

        if (!connection->callback->Invoke(args))
            allInvoked = false;
    }

    return allInvoked;
}

} // namespace URay
