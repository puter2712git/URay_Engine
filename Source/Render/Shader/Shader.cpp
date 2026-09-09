#include "Shader.h"

#include "Core/Type/Types.h"

#include <cassert>
#include <map>

namespace URay::Render
{

namespace
{

ResourceType ToResourceType(ShaderResourceType type)
{
    switch (type)
    {
    case ShaderResourceType::Sampler:
        return ResourceType::Sampler;
    case ShaderResourceType::CombinedImageSampler:
        return ResourceType::CombinedImageSampler;
    case ShaderResourceType::SampledImage:
        return ResourceType::SampledImage;
    case ShaderResourceType::StorageImage:
        return ResourceType::StorageImage;
    case ShaderResourceType::UniformBuffer:
        return ResourceType::UniformBuffer;
    case ShaderResourceType::StorageBuffer:
        return ResourceType::StorageBuffer;
    }

    return ResourceType::Sampler;
}

} // namespace

Shader::Shader(const std::vector<uint8>& vertexShaderCode,
               const std::vector<uint8>& fragmentShaderCode,
               const ShaderReflection& vertexReflection,
               const ShaderReflection& fragmentReflection)
    : vertexShaderCode(vertexShaderCode), fragmentShaderCode(fragmentShaderCode),
      vertexReflection(vertexReflection), fragmentReflection(fragmentReflection)
{
    std::map<std::pair<uint32, uint32>, ReflectedDescriptorBinding> mergedBindings;
    std::map<std::pair<uint32, uint32>, MergedPushConstantBlock> mergedPushConstants;

    auto MergeReflection = [&](const ShaderReflection& reflection)
    {
        assert(reflection.stage != ShaderStageFlags::None);

        for (const ReflectedDescriptorBinding& reflected : reflection.descriptorBindings)
        {
            assert(!reflected.isRuntimeArray);
            assert(reflected.arrayCount > 0);

            ReflectedDescriptorBinding binding = reflected;
            binding.stages = reflection.stage;

            const std::pair<uint32, uint32> key = {
                binding.set,
                binding.binding,
            };
            auto [it, inserted] = mergedBindings.insert({ key, binding });

            if (!inserted)
            {
                assert(it->second.resourceType == binding.resourceType);
                assert(it->second.arrayCount == binding.arrayCount);
                assert(it->second.isRuntimeArray == binding.isRuntimeArray);
                assert(it->second.sampledScalarType == binding.sampledScalarType);
                assert(it->second.readOnly == binding.readOnly);
                assert(it->second.writeOnly == binding.writeOnly);

                it->second.stages = it->second.stages | binding.stages;
            }
        }
    };

    auto MergePushConstants = [&](const ShaderReflection& reflection)
    {
        for (const ReflectedPushConstantBlock& block : reflection.pushConstantBlocks)
        {
            if (block.size == 0)
                continue;

            const std::pair<uint32, uint32> key = {
                block.offset,
                block.size,
            };
            auto [it, inserted] = mergedPushConstants.insert({
                key,
                MergedPushConstantBlock{
                    .block = block,
                    .stages = reflection.stage,
                },
            });

            if (!inserted)
                it->second.stages = it->second.stages | reflection.stage;
        }
    };

    MergeReflection(vertexReflection);
    MergeReflection(fragmentReflection);

    MergePushConstants(vertexReflection);
    MergePushConstants(fragmentReflection);

    for (const auto& [key, binding] : mergedBindings)
    {
        pipelineReflection.descriptorBindings.push_back(binding);

        ResourceBinding rhiBinding = {};
        rhiBinding.set = binding.set;
        rhiBinding.bindingIndex = binding.binding;
        rhiBinding.resourceType = ToResourceType(binding.resourceType);
        rhiBinding.arrayCount = binding.arrayCount;
        rhiBinding.stageFlags = binding.stages;

        setLayoutDescs[key.first].bindings.push_back(rhiBinding);
    }

    for (const auto& [key, block] : mergedPushConstants)
    {
        pipelineReflection.pushConstantBlocks.push_back(block);

        PushConstantRange range = {};
        range.offset = block.block.offset;
        range.size = block.block.size;
        range.stages = block.stages;
        pushConstantRanges.push_back(range);
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
