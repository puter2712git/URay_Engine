#pragma once

#include "Render/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/PushConstantRange.h"

#include <spirv/spirv_reflect.h>

#include <map>
#include <vector>

namespace URay
{

struct ShaderReflection;

class ShaderReflector
{
public:
    static bool ReflectSPIRV(
        const std::vector<uint8_t>& code,
        ShaderReflection& outReflection);

private:
    static bool CreateDescriptorSetLayoutDesc(
        const SpvReflectShaderModule& module,
        std::map<uint32_t, DescriptorSetLayoutDesc>& outDescs);

    static bool CreatePushConstantRange(
        const SpvReflectShaderModule& module,
        PushConstantRange& outRange);
};

} // namespace URay
