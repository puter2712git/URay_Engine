#pragma once

#include "Engine/Component/Component.h"

#include <functional>
#include <memory>
#include <string>

namespace URay
{

class Component;

class ComponentFactory
{
public:
    using ConstructorFunc = std::function<std::unique_ptr<Component>()>;

    template <typename T>
    static void RegisterComponent(const std::string& name)
    {
        GetRegistry()[name] = []() -> std::unique_ptr<Component>
        {
            return std::make_unique<T>();
        };
    }

    static std::unique_ptr<Component> Create(const std::string& name)
    {
        auto& registry = GetRegistry();
        auto it = registry.find(name);

        if (it == registry.end())
            return nullptr;

        return it->second();
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
