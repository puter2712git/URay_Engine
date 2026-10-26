#include "Class.h"

#include <Engine/Object/Class/ClassRegistry.h>

namespace URay
{

Class::Class(const Type& type) : type(type) {}

Class::~Class() = default;

std::vector<Property> Class::GetAllProperties() const
{
    std::vector<Property> result;

    const Type& type = GetType();
    const Type* parentType = type.parent;
    if (parentType)
    {
        if (Class* parentClass = ClassRegistry::Get().Find(*parentType))
            result = parentClass->GetAllProperties();
    }

    result.insert(result.end(), properties.begin(), properties.end());
    return result;
}

} // namespace URay
