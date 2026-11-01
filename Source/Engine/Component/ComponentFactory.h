#pragma once
#include <Core/Type/TypeAlias.h>

#include <Engine/Component/Component.h>

#include <functional>
#include <memory>

namespace URay
{

class Component;

class ComponentFactory
{
public:
    using ConstructorFunc = std::function<UniquePtr<Component>()>;

    template <typename T>
    static void RegisterComponent(const String& name)
    {
        GetRegistry()[name] = []() -> UniquePtr<Component>
        {
            return MakeUnique<T>();
        };
    }

    static UniquePtr<Component> Create(const String& name)
    {
        auto& registry = GetRegistry();
        auto it = registry.find(name);

        if (it == registry.end())
            return nullptr;

        return it->second();
    }

    static const std::unordered_map<String, ConstructorFunc>& GetRegisteredComponents()
    {
        return GetRegistry();
    }

private:
    static std::unordered_map<String, ConstructorFunc>& GetRegistry()
    {
        static std::unordered_map<String, ConstructorFunc> registry;
        return registry;
    }
};

} // namespace URay
