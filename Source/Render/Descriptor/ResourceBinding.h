#pragma once

#include <cstdint>

namespace URay
{

enum class ResourceType
{
    ConstantBuffer,
    SampledImage,
    Sampler
};

enum class ShaderStageFlags : uint32_t
{
    Vertex = 1 << 0,
    Fragment = 1 << 1,
    All = (1 << 0) | (1 << 1)
};

constexpr ShaderStageFlags operator|(ShaderStageFlags lhs, ShaderStageFlags rhs)
{
    return static_cast<ShaderStageFlags>(
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr ShaderStageFlags operator&(ShaderStageFlags lhs, ShaderStageFlags rhs)
{
    return static_cast<ShaderStageFlags>(
        static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

struct ResourceBinding
{
    uint32_t bindingIndex = 0;
    ResourceType resourceType = ResourceType::ConstantBuffer;
    uint32_t arrayCount = 1;
    ShaderStageFlags stageFlags = ShaderStageFlags::Vertex;

    bool operator==(const ResourceBinding&) const = default;
};

} // namespace URay
