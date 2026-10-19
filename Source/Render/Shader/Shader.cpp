#include "Shader.h"

#include <Core/Type/Types.h>

#include <cassert>
#include <map>

namespace URay::Render
{

Shader::Shader(std::vector<ShaderStageData> stages)
{
    for (const ShaderStageData& stage : stages)
    {
        if (stage.stage == ShaderStageFlags::Vertex)
        {
            vertexStageData = std::make_unique<ShaderStageData>(stage);
        }
        else if (stage.stage == ShaderStageFlags::Fragment)
        {
            fragmentStageData = std::make_unique<ShaderStageData>(stage);
        }
        else if (stage.stage == ShaderStageFlags::Compute)
        {
            computeStageData = std::make_unique<ShaderStageData>(stage);
        }
    }

    MergeReflection();
    CreateSetLayoutDescriptions();

    if (mergedReflection.pushConstant.size > 0)
    {
        pushConstantRanges.push_back(PushConstantRange{
            .offset = mergedReflection.pushConstant.offset,
            .size = mergedReflection.pushConstant.size,
            .stages = mergedReflection.pushConstant.stageFlags });
    }
}

Shader::~Shader() = default;

bool Shader::HasStage(ShaderStageFlags stage) const
{
    if (stage == ShaderStageFlags::Vertex)
        return vertexStageData != nullptr;
    else if (stage == ShaderStageFlags::Fragment)
        return fragmentStageData != nullptr;
    else if (stage == ShaderStageFlags::Compute)
        return computeStageData != nullptr;

    return false;
}

const ShaderStageData* Shader::GetStageData(ShaderStageFlags stage) const
{
    if (stage == ShaderStageFlags::Vertex)
        return vertexStageData.get();
    else if (stage == ShaderStageFlags::Fragment)
        return fragmentStageData.get();
    else if (stage == ShaderStageFlags::Compute)
        return computeStageData.get();

    return nullptr;
}

bool Shader::IsGraphics() const
{
    return vertexStageData != nullptr && fragmentStageData != nullptr;
}

bool Shader::IsCompute() const
{
    return computeStageData != nullptr;
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
    std::vector<ShaderReflection*> reflections;

    if (vertexStageData)
    {
        reflections.push_back(&vertexStageData->reflection);
    }
    if (fragmentStageData)
    {
        reflections.push_back(&fragmentStageData->reflection);
    }
    if (computeStageData)
    {
        reflections.push_back(&computeStageData->reflection);
    }

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
    std::vector<ShaderReflection*> reflections;

    if (vertexStageData)
    {
        reflections.push_back(&vertexStageData->reflection);
    }
    if (fragmentStageData)
    {
        reflections.push_back(&fragmentStageData->reflection);
    }
    if (computeStageData)
    {
        reflections.push_back(&computeStageData->reflection);
    }

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
    const ShaderStageData* stages[] = { vertexStageData.get(), fragmentStageData.get(), computeStageData.get() };

    for (const ShaderStageData* stageData : stages)
    {
        if (!stageData)
            continue;

        const ShaderPushConstant& pushConstant = stageData->reflection.pushConstant;

        if (pushConstant.size == 0)
            continue;

        if (mergedReflection.pushConstant.size == 0)
        {
            mergedReflection.pushConstant = pushConstant;
        }
        else
        {
            assert(mergedReflection.pushConstant.offset == pushConstant.offset);
            assert(mergedReflection.pushConstant.size == pushConstant.size);
            assert(mergedReflection.pushConstant.parameters == pushConstant.parameters);
        }

        mergedReflection.pushConstant.stageFlags |= pushConstant.stageFlags;
    }
}

ShaderDescriptorBinding* Shader::FindBinding(uint32 set, uint32 binding)
{
    for (ShaderDescriptorBinding& descriptorBinding : mergedReflection.descriptorBindings)
    {
        if (descriptorBinding.set == set && descriptorBinding.binding == binding)
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
