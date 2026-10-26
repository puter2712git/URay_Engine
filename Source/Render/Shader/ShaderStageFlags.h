#pragma once

#include <Core/Type/TypeAlias.h>

#include <cstdint>
#include <vulkan/vulkan.h>

namespace URay::Render
{

enum class ShaderStageFlags : uint32
{
    None = 0,
    Vertex = 1 << 0,
    Fragment = 1 << 1,
    Compute = 1 << 2,

    Graphics = (1 << 0) | (1 << 1),
    All = (1 << 0) | (1 << 1) | (1 << 2)
};

inline ShaderStageFlags operator|(ShaderStageFlags lhs, ShaderStageFlags rhs)
{
    return static_cast<ShaderStageFlags>(
        static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

inline ShaderStageFlags operator&(ShaderStageFlags lhs, ShaderStageFlags rhs)
{
    return static_cast<ShaderStageFlags>(
        static_cast<uint32>(lhs) & static_cast<uint32>(rhs));
}

inline ShaderStageFlags& operator|=(ShaderStageFlags& lhs, ShaderStageFlags rhs)
{
    return lhs = lhs | rhs;
}

inline ShaderStageFlags& operator&=(ShaderStageFlags& lhs, ShaderStageFlags rhs)
{
    return lhs = lhs & rhs;
}

inline ShaderStageFlags operator~(ShaderStageFlags value)
{
    return static_cast<ShaderStageFlags>(~static_cast<uint32>(value));
}

inline VkShaderStageFlags ToVkShaderStageFlags(ShaderStageFlags flags)
{
    VkShaderStageFlags result = 0;

    if ((flags & ShaderStageFlags::Vertex) != ShaderStageFlags::None)
        result |= VK_SHADER_STAGE_VERTEX_BIT;

    if ((flags & ShaderStageFlags::Fragment) != ShaderStageFlags::None)
        result |= VK_SHADER_STAGE_FRAGMENT_BIT;

    if ((flags & ShaderStageFlags::Compute) != ShaderStageFlags::None)
        result |= VK_SHADER_STAGE_COMPUTE_BIT;

    return result;
};

} // namespace URay::Render
