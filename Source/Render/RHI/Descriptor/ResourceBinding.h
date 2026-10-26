#pragma once
#include <Core/Type/TypeAlias.h>

#include <Render/Shader/ShaderStageFlags.h>


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
    String name;
    uint32 binding = 0;
    ResourceType resourceType = ResourceType::Sampler;
    uint32 arrayCount = 1;
    ShaderStageFlags stageFlags = ShaderStageFlags::Vertex;

    bool operator==(const ResourceBinding&) const = default;
};

} // namespace URay::Render
