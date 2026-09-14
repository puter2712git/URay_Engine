#pragma once

#include "Render/Shader/ShaderStageFlags.h"

#include "Core/Type/Types.h"

#include <cstdint>
#include <string>

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
    std::string name;
    uint32 binding = 0;
    ResourceType resourceType = ResourceType::Sampler;
    uint32 arrayCount = 1;
    ShaderStageFlags stageFlags = ShaderStageFlags::Vertex;

    bool operator==(const ResourceBinding&) const = default;
};

} // namespace URay::Render
