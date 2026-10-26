#pragma once

#include <unordered_map>

namespace URay
{

class Class;
struct Type;

class ClassRegistry
{
public:
    static ClassRegistry& Get()
    {
        static ClassRegistry instance;
        return instance;
    }

    void Register(Class& cls);
    Class* Find(const Type& type) const;

private:
    std::unordered_map<const Type*, Class*> classes;
};

} // namespace URay
