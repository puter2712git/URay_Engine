#pragma once

#include "PropertyTypes.h"

#include <string>

namespace URay
{

struct Property
{
    PropertyType type = PropertyType::Bool;

    std::string name;
    size_t offset;
    size_t size;

    Property(PropertyType type, const std::string& name, size_t offset, size_t size);

    template <typename T>
    T GetValue(void* address) const
    {
        const void* valueAddress = reinterpret_cast<const char*>(address) + offset;
        T value = *static_cast<const T*>(valueAddress);
        return value;
    }
};

} // namespace URay
