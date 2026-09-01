#include "Shader.h"

#include "Core/Type/Types.h"

#include <cassert>
#include <map>

namespace URay::Render
{

Shader::Shader(uint32 id,
               const ShaderStage& vertexStage,
               const ShaderStage& fragmentStage,
               const ShaderReflection& vertexReflection,
               const ShaderReflection& fragmentReflection)
    : id(id),
      vertexStage(vertexStage), fragmentStage(fragmentStage),
      vertexReflection(vertexReflection), fragmentReflection(fragmentReflection)
{
    std::map<std::pair<uint32, uint32>, ResourceBinding> mergedBindings;

    auto Merge = [&](const std::map<uint32, DescriptorSetLayoutDesc>& descs)
    {
        for (auto& [set, desc] : descs)
        {
            for (const auto& binding : desc.bindings)
            {
                std::pair<uint32, uint32> key = { binding.set,
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

    auto MergePushConstantRange = [&](const PushConstantRange& range)
    {
        if (range.size == 0)
            return;

        for (PushConstantRange& existing : pushConstantRanges)
        {
            if (existing.offset == range.offset &&
                existing.size == range.size)
            {
                existing.stages = existing.stages | range.stages;
                return;
            }
        }

        pushConstantRanges.push_back(range);
    };

    Merge(vertexReflection.setLayoutDescs);
    Merge(fragmentReflection.setLayoutDescs);

    MergePushConstantRange(vertexReflection.pushConstantRange);
    MergePushConstantRange(fragmentReflection.pushConstantRange);

    for (auto& [key, ResourceBinding] : mergedBindings)
    {
        setLayoutDescs[key.first].bindings.push_back(ResourceBinding);
    }
}

const DescriptorSetLayoutDesc* Shader::GetDescriptorSetLayoutDesc(uint32 set) const
{
    auto it = setLayoutDescs.find(set);
    if (it != setLayoutDescs.end())
    {
        return &it->second;
    }

    return nullptr;
}

} // namespace URay::Render
