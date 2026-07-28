#pragma once

#include <spirv/spirv_reflect.h>

#include <vector>

namespace URay
{

struct ShaderReflectionContext
{
    std::vector<SpvReflectDescriptorSet*> sets;
    std::vector<SpvReflectBlockVariable*> pushBlocks;
    std::vector<SpvReflectInterfaceVariable*> inputs;
};

class ShaderReflector
{
public:
    static bool ReflectSPIRV(
        const std::vector<uint8_t>& code,
        ShaderReflectionContext& outContext);
};

} // namespace URay
