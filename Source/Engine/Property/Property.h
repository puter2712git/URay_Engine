#pragma once

#include "PropertyTypes.h"

#include <string>

namespace URay
{

struct Property
{
    PropertyType type = PropertyType::Bool;

    std::string name;
    void* dataPtr = nullptr;

    Property(PropertyType type, const std::string& name, void* data);
};

} // namespace URay
