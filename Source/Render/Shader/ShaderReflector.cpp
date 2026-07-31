#include "ShaderReflector.h"

#include <iostream>

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

    uint32_t setCount = 0;
    spvReflectEnumerateDescriptorSets(&module, &setCount, nullptr);

    std::vector<SpvReflectDescriptorSet*> sets(setCount);
    spvReflectEnumerateDescriptorSets(&module, &setCount, sets.data());

    for (const auto* set : sets)
    {
        outContext.bindings.resize(set->binding_count);

        for (uint32_t i = 0; i < set->binding_count; ++i)
        {
            const auto* binding = set->bindings[i];

            outContext.bindings[i].set = binding->set;
            outContext.bindings[i].binding = binding->binding;

            switch (binding->descriptor_type)
            {
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                outContext.bindings[i].type = ResourceType::ConstantBuffer;
                break;

            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                outContext.bindings[i].type = ResourceType::Sampler;
                break;

            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                outContext.bindings[i].type = ResourceType::SampledImage;
                break;

            default:
                break;
            }

            outContext.bindings[i].descriptorCount = 1;
            outContext.bindings[i].stages = ToShaderStageFlags(module.shader_stage);
        }
    }

    uint32_t pushCount = 0;
    spvReflectEnumeratePushConstantBlocks(&module, &pushCount, nullptr);

    std::vector<SpvReflectBlockVariable*> pushBlocks(pushCount);
    spvReflectEnumeratePushConstantBlocks(&module, &pushCount, pushBlocks.data());

    for (const auto* block : pushBlocks)
    {
        std::cout << "Push Constants: " << block->name
                  << " (Offset: " << block->offset
                  << ", Size: " << block->size << " bytes)\n";
    }

    if (module.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
    {
        uint32_t inputCount = 0;
        spvReflectEnumerateInputVariables(&module, &inputCount, nullptr);

        std::vector<SpvReflectInterfaceVariable*> inputs(inputCount);
        spvReflectEnumerateInputVariables(&module, &inputCount, inputs.data());

        for (const auto* input : inputs)
        {
            if (input->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
                continue;

            std::cout << "Vertex Input Location " << input->location
                      << ": " << input->name
                      << " (Format: " << input->format << ")\n";
        }
    }

    spvReflectDestroyShaderModule(&module);

    return true;
}

} // namespace URay