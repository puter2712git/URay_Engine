#pragma once

#include <Core/Type/Type.h>
#include <Core/Type/TypeAlias.h>
#include <Core/Type/Variant/VariantOperation.h>

namespace URay
{

class Variant
{
public:
    Variant(const char* value) : Variant(String(value)) {}

    template <typename T>
    Variant(const T& value)
        : type(&TypeOf<T>()),
          ops(&ValueVariantPolicy<T>::Operations()),
          value(new T(value))
    {
    }

public:
    template <typename T>
    T* TryGet()
    {
        if (!type || type != &TypeOf<T>())
            return nullptr;

        return static_cast<T*>(value);
    }

    const Type* GetType() const { return type; }

private:
    const Type* type = nullptr;
    const VariantOps* ops = nullptr;
    void* value = nullptr;
};

} // namespace URay
