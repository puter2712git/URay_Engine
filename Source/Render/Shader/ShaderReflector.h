#pragma once

#include "Render/Descriptor/ResourceBinding.h"

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
    std::vector<ReflectedBinding> bindings;
};

class ShaderReflector
{
public:
    static bool ReflectSPIRV(
        const std::vector<uint8_t>& code,
        ShaderReflectionContext& outContext);
};

} // namespace URay
