#pragma once

#include "Render/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/PushConstantRange.h"

#include <spirv/spirv_reflect.h>

#include <vector>

namespace URay
{

struct ReflectedBinding
{
    uint32_t set = 0;
    uint32_t binding = 0;
    ResourceType type = ResourceType::ConstantBuffer;
    uint32_t descriptorCount = 0;
    ShaderStageFlags stages = ShaderStageFlags::Vertex;
};

struct ShaderReflectionContext
{
    DescriptorSetLayoutDesc setLayoutDesc = {};
    PushConstantRange pushConstantRange = {};
};

class ShaderReflector
{
public:
    static bool ReflectSPIRV(
        const std::vector<uint8_t>& code,
        ShaderReflectionContext& outContext);

private:
    static bool CreateDescriptorSetLayoutDesc(
        const SpvReflectShaderModule& module,
        DescriptorSetLayoutDesc& outDesc);

    static bool CreatePushConstantRange(
        const SpvReflectShaderModule& module,
        PushConstantRange& outRange);
};

} // namespace URay
