#include "Sampler.h"

#include <Render/RHI/Device.h>

namespace URay::Render
{

Sampler::Sampler(Device& device, VkSampler handle, const SamplerDesc& desc)
    : device(device), handle(handle), desc(desc)
{
}

Sampler::~Sampler()
{
    if (handle != VK_NULL_HANDLE)
    {
        vkDestroySampler(device.GetVKDevice(), handle, nullptr);
    }
}

} // namespace URay::Render
