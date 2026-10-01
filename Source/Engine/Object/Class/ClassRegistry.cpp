#include "ClassRegistry.h"

#include "Engine/Object/Class/Class.h"

namespace URay
{

void ClassRegistry::Register(Class& cls)
{
    classes.insert_or_assign(&cls.GetType(), &cls);
}

Class* ClassRegistry::Find(const RuntimeType& type) const
{
    const auto it = classes.find(&type);
    if (it == classes.end())
        return nullptr;

    return it->second;
}

} // namespace URay
