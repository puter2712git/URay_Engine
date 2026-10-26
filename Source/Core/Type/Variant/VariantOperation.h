#pragma once

namespace URay
{

struct VariantOps
{
    void* (*copy)(const void* src);
    void (*destroy)(void* value) noexcept;
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

    static const VariantOps& Operations()
    {
        static const VariantOps ops{
            .copy = &Copy,
            .destroy = &Destroy
        };
        return ops;
    }
};

} // namespace URay
