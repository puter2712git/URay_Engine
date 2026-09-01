#pragma once

#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/PushConstantRange.h"

#include "Core/Type/Types.h"

#include <spirv/spirv_reflect.h>
#include <map>
#include <vector>

namespace URay::Render
{

struct ShaderReflection;

class ShaderReflector
{
public:
    static bool ReflectSPIRV(
        const std::vector<uint8>& code,
        ShaderReflection& outReflection);

private:
    static bool CreateDescriptorSetLayoutDesc(
        const SpvReflectShaderModule& module,
        std::map<uint32, DescriptorSetLayoutDesc>& outDescs);

    static bool CreatePushConstantRange(
        const SpvReflectShaderModule& module,
        PushConstantRange& outRange);
};

} // namespace URay::Render
