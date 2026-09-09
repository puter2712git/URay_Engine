#pragma once

#include "Render/Shader/ShaderStageFlags.h"

#include "Core/Type/Types.h"

#include <cstdint>

namespace URay::Render
{

enum class ResourceType
{
    Sampler,
    CombinedImageSampler,
    SampledImage,
    StorageImage,
    UniformBuffer,
    StorageBuffer,
};

struct ResourceBinding
{
    uint32 set = 0;
    uint32 bindingIndex = 0;
    ResourceType resourceType = ResourceType::Sampler;
    uint32 arrayCount = 1;
    ShaderStageFlags stageFlags = ShaderStageFlags::Vertex;

    bool operator==(const ResourceBinding&) const = default;
};

} // namespace URay::Render
