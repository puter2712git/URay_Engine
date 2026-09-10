#include "Class.h"

namespace URay
{

Class::Class(const std::string& name, Class* superClass)
    : name(name), superClass(superClass)
{
}

Class::~Class() = default;

std::vector<Property> Class::GetAllProperties() const
{
    std::vector<Property> result;

    if (superClass)
        result = superClass->GetAllProperties();

    result.insert(result.end(), properties.begin(), properties.end());
    return result;
}

} // namespace URay
