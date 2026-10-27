#pragma once

#include <Core/Type/Type.h>
#include <Core/Type/TypeAlias.h>
#include <Core/Type/Variant/VariantOperation.h>

namespace URay
{

class Object;

class Variant
{
public:
    Variant(const char* value) : Variant(String(value)) {}

    Variant(Object* object);

    template <typename T>
        requires(!std::is_pointer_v<T>)
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
        if (!type || type->isObjectType || type != &TypeOf<T>())
            return nullptr;

        return static_cast<T*>(value);
    }

    template <typename T>
    T* TryGetObject() const
    {
        return Cast<T>(ResolveObject());
    }

    const Type* GetType() const { return type; }

private:
    Object* ResolveObject() const;

private:
    const Type* type = nullptr;
    const VariantOps* ops = nullptr;
    void* value = nullptr;
};

} // namespace URay
