#pragma once

#include <utility>

namespace URay
{

template <typename T, typename... Args>
T* MemNew(Args&&... args)
{
    T* ptr = new T(std::forward<Args>(args)...);
    return ptr;
}

template <typename T>
void MemDelete(T* ptr) noexcept
{
    if (!ptr)
        return;

    delete ptr;
}

} // namespace URay
