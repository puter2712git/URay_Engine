#include "Class.h"

namespace URay
{

Class::Class(const std::string& name, Class* superClass)
    : name(name), superClass(superClass)
{
}

Class::~Class() = default;

} // namespace URay
