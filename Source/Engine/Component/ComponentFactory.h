#pragma once
#include <Core/Type/Types.h>

#include <Engine/Component/Component.h>

#include <functional>
#include <memory>

namespace URay
{

class Component;

class ComponentFactory
{
public:
    using ConstructorFunc = std::function<std::unique_ptr<Component>()>;

    template <typename T>
    static void RegisterComponent(const String& name)
    {
        GetRegistry()[name] = []() -> std::unique_ptr<Component>
        {
            return std::make_unique<T>();
        };
    }

    static std::unique_ptr<Component> Create(const String& name)
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
