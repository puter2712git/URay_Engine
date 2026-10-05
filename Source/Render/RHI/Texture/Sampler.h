#pragma once

#include <vulkan/vulkan.h>

#include <functional>

namespace URay::Render
{

class Device;

struct SamplerDesc
{
    VkFilter magFilter = VK_FILTER_LINEAR;
    VkFilter minFilter = VK_FILTER_LINEAR;

    VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    bool operator==(const SamplerDesc&) const = default;
};

struct SamplerDescHash
{
    size_t operator()(const SamplerDesc& desc) const
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

class Sampler
{
public:
    Sampler(Device& device, VkSampler handle, const SamplerDesc& desc);
    ~Sampler();

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;
    Sampler(Sampler&&) = delete;
    Sampler& operator=(Sampler&&) = delete;

public:
    VkSampler GetHandle() const { return handle; }
    const SamplerDesc& GetDesc() const { return desc; }

private:
    Device& device;
    VkSampler handle = VK_NULL_HANDLE;
    SamplerDesc desc = {};
};

} // namespace URay::Render
