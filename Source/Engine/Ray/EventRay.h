#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <utility>

namespace URay
{

using RayHandle = std::uint32_t;

template <typename... Args>
class EventRay
{
public:
    using Callback = std::function<void(Args...)>;

public:
    RayHandle Register(Callback callback);
    void Unregister(RayHandle handle);

    void Emit(Args... args);

private:
    RayHandle nextHandle = 0;
    std::unordered_map<RayHandle, Callback> callbacks;
};

} // namespace URay

#include "EventRay.tpp"
