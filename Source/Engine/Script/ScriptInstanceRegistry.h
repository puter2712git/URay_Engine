#pragma once

#include <Core/Type/Types.h>

#include <unordered_map>
#include <vector>

namespace URay
{

class ScriptComponent;

class ScriptInstanceRegistry
{
public:
    bool Bind(uint64 instanceId, ScriptComponent& component);
    void Unbind(uint64 instanceId, const ScriptComponent& component);

    ScriptComponent* Find(uint64 instanceId) const;

    std::vector<ScriptComponent*> GetBoundComponents() const;

    bool Empty() const;
    void Clear();

private:
    std::unordered_map<uint64, ScriptComponent*> instances;
};

} // namespace URay
