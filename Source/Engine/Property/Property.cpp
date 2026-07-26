#include "Property.h"

namespace URay
{

Property::Property(PropertyType type, const std::string& name, void* data)
    : type(type), name(name), dataPtr(data)
{
}

} // namespace URay
