#include "ScriptInstanceRegistry.h"

#include <Engine/Component/ScriptComponent.h>

namespace URay
{

bool ScriptInstanceRegistry::Bind(uint64 instanceId, ScriptComponent& component)
{
    if (instanceId == 0 || !component.GetOwner())
        return false;

    if (instances.contains(instanceId))
        return false;

    instances.emplace(instanceId, &component);
    return true;
}

void ScriptInstanceRegistry::Unbind(uint64 instanceId, const ScriptComponent& component)
{
    const auto it = instances.find(instanceId);

    if (it == instances.end() || it->second != &component)
        return;

    instances.erase(it);
}

ScriptComponent* ScriptInstanceRegistry::Find(uint64 instanceId) const
{
    const auto it = instances.find(instanceId);
    return it != instances.end() ? it->second : nullptr;
}

std::vector<ScriptComponent*> ScriptInstanceRegistry::GetBoundComponents() const
{
    std::vector<ScriptComponent*> components;
    components.reserve(instances.size());

    for (const auto& entry : instances)
    {
        ScriptComponent* component = entry.second;

        if (component)
            components.push_back(component);
    }

    return components;
}

bool ScriptInstanceRegistry::Empty() const
{
    return instances.empty();
}

void ScriptInstanceRegistry::Clear()
{
    instances.clear();
}

} // namespace URay
