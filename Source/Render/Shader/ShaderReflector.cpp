#include "ShaderReflector.h"

#include <iostream>

namespace URay
{

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

    outContext.sets.resize(setCount);
    spvReflectEnumerateDescriptorSets(&module, &setCount, outContext.sets.data());

    for (const auto* set : outContext.sets)
    {
        std::cout << "Set " << set->set << " (Binding Count: " << set->binding_count << ")\n";
        for (uint32_t i = 0; i < set->binding_count; ++i)
        {
            const auto* binding = set->bindings[i];
            std::cout << " - Binding " << binding->binding
                      << ": " << binding->name
                      << " (Type: " << binding->descriptor_type << ")\n";
        }
    }

    uint32_t pushCount = 0;
    spvReflectEnumeratePushConstantBlocks(&module, &pushCount, nullptr);

    outContext.pushBlocks.resize(pushCount);
    spvReflectEnumeratePushConstantBlocks(&module, &pushCount, outContext.pushBlocks.data());

    for (const auto* block : outContext.pushBlocks)
    {
        std::cout << "Push Constants: " << block->name
                  << " (Offset: " << block->offset
                  << ", Size: " << block->size << " bytes)\n";
    }

    if (module.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
    {
        uint32_t inputCount = 0;
        spvReflectEnumerateInputVariables(&module, &inputCount, nullptr);

        outContext.inputs.resize(inputCount);
        spvReflectEnumerateInputVariables(&module, &inputCount, outContext.inputs.data());

        for (const auto* input : outContext.inputs)
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