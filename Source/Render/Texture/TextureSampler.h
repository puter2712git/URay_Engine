#pragma once

#include <vulkan/vulkan.h>

#include <functional>

namespace URay::Render
{

struct TextureSamplerDesc
{
    VkFilter magFilter = VK_FILTER_LINEAR;
    VkFilter minFilter = VK_FILTER_LINEAR;

    VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    bool operator==(const TextureSamplerDesc&) const = default;
};

struct TextureSamplerDescHash
{
    size_t operator()(const TextureSamplerDesc& desc) const
    {
        size_t hash = std::hash<VkFilter>{}(desc.magFilter);

        auto combine = [&hash](auto value)
        {
            size_t valueHash = std::hash<decltype(value)>{}(value);
            hash ^= valueHash + 0x9e3779b9 + (hash << 6) + (hash << 2);
        };

        combine(desc.minFilter);
        combine(desc.addressModeU);
        combine(desc.addressModeV);
        combine(desc.addressModeW);

        return hash;
    }
};

} // namespace URay::Render
