#pragma once

#include "PropertyTypes.h"

#include <functional>
#include <string>

namespace URay
{

class Object;

struct Property
{
    PropertyType type = PropertyType::Bool;

    std::string name;
    size_t offset = 0;
    size_t size = 0;
    std::function<void(Object* owner, const Property& property)> OnChangedCallback = nullptr;

    Property(PropertyType type, const std::string& name, size_t offset, size_t size,
             std::function<void(Object* owner, const Property& property)> OnChanged = nullptr);

    template <typename T>
    T GetValue(void* address) const
    {
        const void* valueAddress = reinterpret_cast<const char*>(address) + offset;
        T value = *static_cast<const T*>(valueAddress);
        return value;
    }
};

} // namespace URay
