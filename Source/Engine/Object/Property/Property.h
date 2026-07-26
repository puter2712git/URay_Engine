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
};

} // namespace URay
