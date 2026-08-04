#include "Property.h"

namespace URay
{

Property::Property(PropertyType type, const std::string& name, size_t offset, size_t size,
                   std::function<void(Object* owner, const Property& property)> OnChanged)
    : type(type), name(name), offset(offset), size(size), OnChangedCallback(OnChanged)
{
}

} // namespace URay
