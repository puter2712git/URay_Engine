#include "Property.h"

namespace URay
{

Property::Property(PropertyType type, const std::string& name, size_t offset, size_t size)
    : type(type), name(name), offset(offset), size(size)
{
}

} // namespace URay
