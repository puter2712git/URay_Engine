#pragma once

#include <Core/Type/Type.h>
#include <Core/Type/TypeAlias.h>
#include <Core/Type/Variant/VariantOperation.h>

#include <stdexcept>
#include <type_traits>
#include <utility>

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
        : type(&TypeOf<T>())
    {
        if (type->kind != TypeKind::Value)
            throw std::invalid_argument("Variant value must have TypeKind::Value");

        ops = &ValueVariantPolicy<T>::Operations();
        this->value = new T(value);
    }

    template <typename T>
        requires(
            std::is_class_v<std::remove_cv_t<T>> &&
            !std::is_volatile_v<T> &&
            !std::is_convertible_v<T*, const Object*>)
    Variant(T* pointer)
        : type(&TypeOf<std::remove_cv_t<T>>())
    {
        if (type->kind != TypeKind::Class || type->isObjectType)
            throw std::invalid_argument("Variant pointer must refer to a non-Object class");

        ops = &ValueVariantPolicy<T*>::Operations();
        value = new T*(pointer);
    }

    template <typename T>
        requires std::is_convertible_v<T*, Object*>
    Variant(T* object) : Variant(static_cast<Object*>(object))
    {
        if (!object)
            type = &TypeOf<T>();
    }

    Variant(const Variant&) = delete;
    Variant& operator=(const Variant&) = delete;

    Variant(Variant&& other) noexcept
        : type(std::exchange(other.type, nullptr)),
          ops(std::exchange(other.ops, nullptr)),
          value(std::exchange(other.value, nullptr)) {}

    Variant& operator=(Variant&& other) noexcept
    {
        if (this != &other)
        {
            Reset();

            type = std::exchange(other.type, nullptr);
            ops = std::exchange(other.ops, nullptr);
            value = std::exchange(other.value, nullptr);
        }

        return *this;
    }

    ~Variant() { Reset(); }

public:
    template <typename T>
    T* TryGet()
    {
        if (!type || type->kind != TypeKind::Value || type != &TypeOf<T>())
            return nullptr;

        return static_cast<T*>(value);
    }

    template <typename T>
    const T* TryGet() const
    {
        if (!type || type->kind != TypeKind::Value || type != &TypeOf<T>())
        {
            return nullptr;
        }

        return static_cast<const T*>(value);
    }

    template <typename T>
    T* TryGetPointer() const
    {
        using ClassType = std::remove_cv_t<T>;

        if (!type ||
            type->kind != TypeKind::Class ||
            type->isObjectType ||
            type != &TypeOf<ClassType>() ||
            ops != &ValueVariantPolicy<T*>::Operations())
        {
            return nullptr;
        }

        return *static_cast<T* const*>(value);
    }

    template <typename T>
    T* TryGetObject() const
    {
        return Cast<T>(ResolveObject());
    }

    const Type* GetType() const { return type; }

    bool IsConstPointer() const noexcept;
    bool IsNullPointer() const noexcept;
    bool IsNullObject() const noexcept;

private:
    void Reset() noexcept;

    Object* ResolveObject() const;

private:
    const Type* type = nullptr;
    const VariantOps* ops = nullptr;
    void* value = nullptr;
};

} // namespace URay
