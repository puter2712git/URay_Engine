#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace URay::RHI
{

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

constexpr VkShaderStageFlags ToVkShaderStageFlags(ShaderStageFlags flags)
{
    VkShaderStageFlags result = 0;

    if ((flags & ShaderStageFlags::Vertex) != ShaderStageFlags{})
        result |= VK_SHADER_STAGE_VERTEX_BIT;

    if ((flags & ShaderStageFlags::Fragment) != ShaderStageFlags{})
        result |= VK_SHADER_STAGE_FRAGMENT_BIT;

    return result;
};

} // namespace URay::RHI
