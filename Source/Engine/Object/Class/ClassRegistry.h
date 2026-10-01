#pragma once

#include <unordered_map>

namespace URay
{

class Class;
struct RuntimeType;

class ClassRegistry
{
public:
    static ClassRegistry& Get()
    {
        static ClassRegistry instance;
        return instance;
    }

    void Register(Class& cls);
    Class* Find(const RuntimeType& type) const;

private:
    std::unordered_map<const RuntimeType*, Class*> classes;
};

} // namespace URay
