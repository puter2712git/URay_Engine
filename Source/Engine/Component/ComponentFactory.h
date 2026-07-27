#pragma once

#include "Engine/Component/Component.h"

#include <functional>
#include <string>

namespace URay
{

class Component;

class ComponentFactory
{
public:
    using ConstructorFunc = std::function<Component*()>;

    template <typename T>
    static void RegisterComponent(const std::string& name)
    {
        GetRegistry()[name] = []() -> Component*
        {
            return new T();
        };
    }

    static Component* Create(const std::string& name)
    {
        auto& registry = GetRegistry();
        auto it = registry.find(name);

        if (it != registry.end())
            return it->second();

        return nullptr;
    }

    static const std::unordered_map<std::string, ConstructorFunc>& GetRegisteredComponents()
    {
        return GetRegistry();
    }

private:
    static std::unordered_map<std::string, ConstructorFunc>& GetRegistry()
    {
        static std::unordered_map<std::string, ConstructorFunc> registry;
        return registry;
    }
};

} // namespace URay
