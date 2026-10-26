#pragma once
#include <Core/Type/TypeAlias.h>

#include "PropertyTypes.h"

#include <functional>

namespace URay
{

class Object;

struct Property
{
    PropertyType type = PropertyType::Bool;

    String name;
    size_t offset = 0;
    size_t size = 0;
    std::function<void(Object& owner, const Property& property)> OnChangedCallback = nullptr;

    template <typename T>
    T GetValue(const void* address) const
    {
        const void* valueAddress = reinterpret_cast<const char*>(address) + offset;
        T value = *static_cast<const T*>(valueAddress);
        return value;
    }
};

} // namespace URay
