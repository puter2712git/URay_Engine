#pragma once

#include "Core/Type/Types.h"

#include <functional>
#include <unordered_map>

namespace URay
{

template <typename... Args>
class EventRay
{
public:
    using Callback = std::function<void(Args...)>;

public:
    void Register(const void* owner, Callback callback);
    void UnregisterAll(const void* owner);

    void Emit(Args... args) const;

private:
    std::unordered_map<const void*, std::vector<Callback>> callbacks;
};

} // namespace URay

#include "EventRay.tpp"
