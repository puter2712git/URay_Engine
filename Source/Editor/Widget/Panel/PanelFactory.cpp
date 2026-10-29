#include "PanelFactory.h"

#include <cstdio>
#include <cstdlib>
#include <utility>

namespace URay
{

std::unordered_map<String, PanelRegistration>& PanelFactory::GetRegistry()
{
    static std::unordered_map<String, PanelRegistration> registry;
    return registry;
}

bool PanelFactory::Register(const String& typeName, PanelRegistration registration)
{
    const bool inserted = GetRegistry().emplace(typeName, std::move(registration)).second;
    if (!inserted)
    {
        std::fprintf(stderr, "[PanelFactory] Duplicate panel type: %s\n", typeName.c_str());
        std::abort();
    }
    return inserted;
}

const PanelRegistration* PanelFactory::Find(const String& typeName)
{
    const auto& registry = GetRegistry();
    const auto it = registry.find(typeName);
    return it != registry.end() ? &it->second : nullptr;
}

std::unique_ptr<Panel> PanelFactory::Create(const String& typeName, PanelContext& context)
{
    const PanelRegistration* registration = Find(typeName);
    return registration ? registration->constructor(context) : nullptr;
}

} // namespace URay
