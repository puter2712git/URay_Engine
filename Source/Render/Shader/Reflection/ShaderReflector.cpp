#include "ShaderReflector.h"

#include "Render/RHI/Descriptor/ResourceBinding.h"

#include "Core/Type/Types.h"

#include <utility>

namespace URay::Render
{

namespace
{

bool ToShaderResourceType(
    SpvReflectDescriptorType source,
    ResourceType& outType)
{
    switch (source)
    {
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
        outType = ResourceType::Sampler;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        outType = ResourceType::CombinedImageSampler;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        outType = ResourceType::SampledImage;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        outType = ResourceType::StorageImage;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        outType = ResourceType::UniformBuffer;
        return true;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        outType = ResourceType::StorageBuffer;
        return true;
    default:
        return false;
    }
}

ShaderStageFlags ToShaderStageFlags(SpvReflectShaderStageFlagBits stage)
{
    switch (stage)
    {
    case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
        return ShaderStageFlags::Vertex;
    case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
        return ShaderStageFlags::Fragment;
    default:
        return ShaderStageFlags{};
    }
}

} // namespace

bool ShaderReflector::Reflect(
    const std::vector<uint8>& shaderCode,
    const std::string& entryPoint,
    ShaderReflection& outReflection)
{
    outReflection = {};

    SpvReflectShaderModule module = {};
    if (spvReflectCreateShaderModule(
            shaderCode.size(),
            shaderCode.data(),
            &module) != SPV_REFLECT_RESULT_SUCCESS)
    {
        return false;
    }

    if (!ReflectDescriptorBindings(module, outReflection))
        return false;
    if (!ReflectUniformBuffers(module, outReflection))
        return false;
    if (!ReflectPushConstants(module, outReflection))
        return false;

    spvReflectDestroyShaderModule(&module);

    return true;
}

bool ShaderReflector::ReflectDescriptorBindings(
    const SpvReflectShaderModule& module,
    ShaderReflection& outReflection)
{
    uint32 bindingCount = 0;
    spvReflectEnumerateDescriptorBindings(&module, &bindingCount, nullptr);

    std::vector<SpvReflectDescriptorBinding*> bindings(bindingCount);
    spvReflectEnumerateDescriptorBindings(&module, &bindingCount, bindings.data());

    for (SpvReflectDescriptorBinding* binding : bindings)
    {
        uint32 setIndex = binding->set;
        uint32 bindingIndex = binding->binding;

        SpvReflectDescriptorType descriptorType = binding->descriptor_type;

        ResourceType resourceType = ResourceType::Sampler;
        if (!ToShaderResourceType(descriptorType, resourceType))
            continue;

        const ShaderDescriptorBinding newBinding = {
            .name = binding->name ? binding->name : "",
            .set = setIndex,
            .binding = bindingIndex,
            .type = resourceType,
            .count = binding->count,
            .stageFlags = ToShaderStageFlags(module.shader_stage)
        };
        outReflection.descriptorBindings.push_back(newBinding);
    }

    return true;
}

bool ShaderReflector::ReflectUniformBuffers(
    const SpvReflectShaderModule& module,
    ShaderReflection& outReflection)
{
    uint32 bindingCount = 0;
    spvReflectEnumerateDescriptorBindings(&module, &bindingCount, nullptr);

    std::vector<SpvReflectDescriptorBinding*> bindings(bindingCount);
    spvReflectEnumerateDescriptorBindings(&module, &bindingCount, bindings.data());

    for (SpvReflectDescriptorBinding* binding : bindings)
    {
        SpvReflectDescriptorType descriptorType = binding->descriptor_type;
        if (descriptorType != SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            continue;

        ShaderUniformBuffer uniformBuffer = {};
        uniformBuffer.name = binding->name ? binding->name : "";
        uniformBuffer.set = binding->set;
        uniformBuffer.binding = binding->binding;
        uniformBuffer.size = binding->block.size;
        uniformBuffer.stageFlags = ToShaderStageFlags(module.shader_stage);

        const SpvReflectBlockVariable& block = binding->block;

        for (uint32 i = 0; i < block.member_count; ++i)
        {
            const SpvReflectBlockVariable& member = block.members[i];

            ShaderParameter parameter = {};
            parameter.name = member.name ? member.name : "";
            parameter.offset = member.offset;
            parameter.size = member.size;

            uniformBuffer.parameters.push_back(parameter);
        }

        outReflection.uniformBuffers.push_back(uniformBuffer);
    }

    return true;
}

bool ShaderReflector::ReflectPushConstants(
    const SpvReflectShaderModule& module,
    ShaderReflection& outReflection)
{
    uint32 pushConstantCount = 0;
    spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, nullptr);

    std::vector<SpvReflectBlockVariable*> blocks(pushConstantCount);
    spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, blocks.data());

    ShaderPushConstant& pushConstant = outReflection.pushConstant;

    for (SpvReflectBlockVariable* block : blocks)
    {
        uint32 offset = block->offset;
        uint32 size = block->size;

        pushConstant.offset = block->offset;
        pushConstant.size = block->size;

        for (uint32 i = 0; i < block->member_count; ++i)
        {
            const SpvReflectBlockVariable& member = block->members[i];

            pushConstant.parameters.push_back(ShaderParameter{
                .name = member.name ? member.name : "",
                .offset = member.offset,
                .size = member.size });
        }
    }

    return true;
}

} // namespace URay::Render
