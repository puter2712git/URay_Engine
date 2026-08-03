#include "Shader.h"

#include <cassert>
#include <map>

namespace URay
{

Shader::Shader(uint32_t id,
               const ShaderStage& vertexStage,
               const ShaderStage& fragmentStage,
               const ShaderReflection& vertexReflection,
               const ShaderReflection& fragmentReflection)
    : id(id),
      vertexStage(vertexStage), fragmentStage(fragmentStage),
      vertexReflection(vertexReflection), fragmentReflection(fragmentReflection)
{
    std::map<std::pair<uint32_t, uint32_t>, ResourceBinding> mergedBindings;

    auto Merge = [&](const std::map<uint32_t, DescriptorSetLayoutDesc>& descs)
    {
        for (auto& [set, desc] : descs)
        {
            for (const auto& binding : desc.bindings)
            {
                std::pair<uint32_t, uint32_t> key = { binding.set,
                                                      binding.bindingIndex };

                auto [it, inserted] = mergedBindings.insert({ key, binding });

                if (!inserted)
                {
                    assert(it->second.resourceType == binding.resourceType);
                    assert(it->second.arrayCount == binding.arrayCount);

                    it->second.stageFlags = it->second.stageFlags | binding.stageFlags;
                }
            }
        }
    };

    Merge(vertexReflection.setLayoutDescs);
    Merge(fragmentReflection.setLayoutDescs);

    for (auto& [key, ResourceBinding] : mergedBindings)
    {
        setLayoutDescs[key.first].bindings.push_back(ResourceBinding);
    }
}

const DescriptorSetLayoutDesc* Shader::GetDescriptorSetLayoutDesc(uint32_t set) const
{
    auto it = setLayoutDescs.find(set);
    if (it != setLayoutDescs.end())
    {
        return &it->second;
    }

    return nullptr;
}

} // namespace URay
