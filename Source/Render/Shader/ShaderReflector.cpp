#include "ShaderReflector.h"

namespace URay
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
    const std::vector<uint8_t>& code,
    ShaderReflectionContext& outContext)
{
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(
        code.size() * sizeof(uint8_t),
        code.data(),
        &module);

    if (result != SPV_REFLECT_RESULT_SUCCESS)
        return false;

    if (!CreateDescriptorSetLayoutDesc(module, outContext.setLayoutDesc))
        return false;

    if (module.shader_stage != SPV_REFLECT_SHADER_STAGE_VERTEX_BIT &&
        !CreatePushConstantRange(module, outContext.pushConstantRange))
    {
        return false;
    }

    spvReflectDestroyShaderModule(&module);

    return true;
}

bool ShaderReflector::CreateDescriptorSetLayoutDesc(
    const SpvReflectShaderModule& module,
    DescriptorSetLayoutDesc& outDesc)
{
    outDesc = {};

    uint32_t setCount = 0;
    spvReflectEnumerateDescriptorSets(&module, &setCount, nullptr);

    std::vector<SpvReflectDescriptorSet*> sets(setCount);
    spvReflectEnumerateDescriptorSets(&module, &setCount, sets.data());

    std::vector<ResourceBinding>& bindings = outDesc.bindings;

    for (const auto* set : sets)
    {
        bindings.resize(set->binding_count);

        for (uint32_t i = 0; i < set->binding_count; ++i)
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

            bindings[i].arrayCount = 1;
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

    uint32_t pushCount = 0;
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

} // namespace URay