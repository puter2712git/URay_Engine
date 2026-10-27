#pragma once

#include <type_traits>

namespace URay
{

struct VariantOps
{
    void* (*copy)(const void* src);
    void (*destroy)(void* value) noexcept;

    bool isConstPointer = false;
    bool (*isNullPointer)(const void* value) noexcept = nullptr;
};

template <typename T>
struct ValueVariantPolicy
{
    static void* Copy(const void* src)
    {
        return new T(*static_cast<const T*>(src));
    }

    static void Destroy(void* value) noexcept
    {
        delete static_cast<T*>(value);
    }

    static bool IsNullPointer(const void* value) noexcept
    {
        if constexpr (std::is_pointer_v<T>)
            return *static_cast<const T*>(value) == nullptr;

        return false;
    }

    static const VariantOps& Operations()
    {
        static const VariantOps ops{
            .copy = &Copy,
            .destroy = &Destroy,
            .isConstPointer = std::is_pointer_v<T> && std::is_const_v<std::remove_pointer_t<T>>,
            .isNullPointer = &IsNullPointer
        };

        return ops;
    }
};

} // namespace URay
