#pragma once

#include "Render/Shader/ShaderStageFlags.h"

#include <cstdint>

namespace URay
{

enum class ResourceType
{
    ConstantBuffer,
    SampledImage,
    Sampler
};

struct ResourceBinding
{
    uint32_t set = 0;
    uint32_t bindingIndex = 0;
    ResourceType resourceType = ResourceType::ConstantBuffer;
    uint32_t arrayCount = 1;
    ShaderStageFlags stageFlags = ShaderStageFlags::Vertex;

    bool operator==(const ResourceBinding&) const = default;
};

} // namespace URay
