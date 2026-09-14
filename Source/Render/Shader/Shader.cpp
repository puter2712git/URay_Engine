#include "Shader.h"

#include "Core/Type/Types.h"

#include <cassert>
#include <map>

namespace URay::Render
{

Shader::Shader(
    const std::vector<uint8>& vertexShaderCode, const std::vector<uint8>& fragmentShaderCode,
    const ShaderReflection& vertexReflection, const ShaderReflection& fragmentReflection)
    : vertexShaderCode(vertexShaderCode), fragmentShaderCode(fragmentShaderCode),
      vertexReflection(vertexReflection), fragmentReflection(fragmentReflection)
{
    MergeReflection();
    CreateSetLayoutDescriptions();

    // TODO: Need fix (ShaderStageFlags)
    pushConstantRanges.push_back(PushConstantRange{
        .offset = mergedReflection.pushConstant.offset,
        .size = mergedReflection.pushConstant.size,
        .stages = ShaderStageFlags::All });
}

const DescriptorSetLayoutDesc* Shader::GetLayoutDescription(uint32 set) const
{
    const auto it = layoutDescriptions.find(set);
    if (it == layoutDescriptions.end())
        return nullptr;

    return &it->second;
}

void Shader::MergeReflection()
{
    mergedReflection = {};

    MergeDescriptorBindings();
    MergeUniformBuffers();
    MergePushConstant();
}

void Shader::MergeDescriptorBindings()
{
    std::vector<ShaderReflection*> reflections = { &vertexReflection,
                                                   &fragmentReflection };

    for (const ShaderReflection* reflection : reflections)
    {
        for (const ShaderDescriptorBinding& binding : reflection->descriptorBindings)
        {
            ShaderDescriptorBinding* existing = FindBinding(binding.set, binding.binding);
            if (!existing)
            {
                mergedReflection.descriptorBindings.push_back(binding);
                continue;
            }

            assert(existing->type == binding.type);
            assert(existing->count == binding.count);

            existing->stageFlags |= binding.stageFlags;
        }
    }
}

void Shader::MergeUniformBuffers()
{
    std::vector<ShaderReflection*> reflections = { &vertexReflection,
                                                   &fragmentReflection };

    for (const ShaderReflection* reflection : reflections)
    {
        for (const ShaderUniformBuffer& uniformBuffer : reflection->uniformBuffers)
        {
            ShaderUniformBuffer* existing = FindUniformBuffer(uniformBuffer.set, uniformBuffer.binding);
            if (!existing)
            {
                mergedReflection.uniformBuffers.push_back(uniformBuffer);
                continue;
            }

            existing->stageFlags |= uniformBuffer.stageFlags;
        }
    }
}

void Shader::MergePushConstant()
{
    mergedReflection.pushConstant = vertexReflection.pushConstant;
}

ShaderDescriptorBinding* Shader::FindBinding(uint32 set, uint32 binding)
{
    for (ShaderDescriptorBinding& descriptorBinding :
         mergedReflection.descriptorBindings)
    {
        if (descriptorBinding.set == set &&
            descriptorBinding.binding == binding)
        {
            return &descriptorBinding;
        }
    }

    return nullptr;
}

ShaderUniformBuffer* Shader::FindUniformBuffer(uint32 set, uint32 binding)
{
    for (ShaderUniformBuffer& uniformBuffer :
         mergedReflection.uniformBuffers)
    {
        if (uniformBuffer.set == set &&
            uniformBuffer.binding == binding)
        {
            return &uniformBuffer;
        }
    }

    return nullptr;
}

void Shader::CreateSetLayoutDescriptions()
{
    std::map<uint32, std::vector<ShaderDescriptorBinding*>> bindingsPerSet;

    for (ShaderDescriptorBinding& binding : mergedReflection.descriptorBindings)
    {
        bindingsPerSet[binding.set].push_back(&binding);
    }

    for (auto& [set, bindings] : bindingsPerSet)
    {
        DescriptorSetLayoutDesc description = {};

        for (ShaderDescriptorBinding* descriptorBinding : bindings)
        {
            description.bindings.push_back(ResourceBinding{
                .name = descriptorBinding->name,
                .binding = descriptorBinding->binding,
                .resourceType = descriptorBinding->type,
                .arrayCount = descriptorBinding->count,
                .stageFlags = descriptorBinding->stageFlags });
        }

        layoutDescriptions.insert({ set, description });
    }
}

} // namespace URay::Render
