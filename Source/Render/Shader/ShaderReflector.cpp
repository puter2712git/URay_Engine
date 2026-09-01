#include "Shader.h"
#include "ShaderReflector.h"

#include "Core/Type/Types.h"

namespace URay::Render
{

namespace
{

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

bool ShaderReflector::ReflectSPIRV(
    const std::vector<uint8>& code,
    ShaderReflection& outReflection)
{
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(
        code.size() * sizeof(uint8),
        code.data(),
        &module);

    if (result != SPV_REFLECT_RESULT_SUCCESS)
        return false;

    if (!CreateDescriptorSetLayoutDesc(module, outReflection.setLayoutDescs))
        return false;

    CreatePushConstantRange(module, outReflection.pushConstantRange);

    spvReflectDestroyShaderModule(&module);

    return true;
}

bool ShaderReflector::CreateDescriptorSetLayoutDesc(
    const SpvReflectShaderModule& module,
    std::map<uint32, DescriptorSetLayoutDesc>& outDescs)
{
    outDescs = {};

    uint32 setCount = 0;
    spvReflectEnumerateDescriptorSets(&module, &setCount, nullptr);

    std::vector<SpvReflectDescriptorSet*> sets(setCount);
    spvReflectEnumerateDescriptorSets(&module, &setCount, sets.data());

    for (const auto* set : sets)
    {
        std::vector<ResourceBinding>& bindings = outDescs[set->set].bindings;
        bindings.resize(set->binding_count);

        for (uint32 i = 0; i < set->binding_count; ++i)
        {
            const auto* binding = set->bindings[i];

            bindings[i].set = binding->set;
            bindings[i].bindingIndex = binding->binding;

            switch (binding->descriptor_type)
            {
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                bindings[i].resourceType = ResourceType::ConstantBuffer;
                break;

            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                bindings[i].resourceType = ResourceType::Sampler;
                break;

            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                bindings[i].resourceType = ResourceType::SampledImage;
                break;

            default:
                break;
            }

            bindings[i].arrayCount = binding->count;
            bindings[i].stageFlags = ToShaderStageFlags(module.shader_stage);
        }
    }

    return true;
}

bool ShaderReflector::CreatePushConstantRange(
    const SpvReflectShaderModule& module,
    PushConstantRange& outRange)
{
    outRange = {};

    uint32 pushCount = 0;
    spvReflectEnumeratePushConstantBlocks(&module, &pushCount, nullptr);

    std::vector<SpvReflectBlockVariable*> pushBlocks(pushCount);
    spvReflectEnumeratePushConstantBlocks(&module, &pushCount, pushBlocks.data());

    if (pushBlocks.empty())
        return false;

    for (const auto* block : pushBlocks)
    {
        outRange.offset = block->offset;
        outRange.size = block->size;
        outRange.stages = ToShaderStageFlags(module.shader_stage);
    }

    return true;
}

} // namespace URay::Render
