#include "DescriptorSetLayout.h"

namespace URay::RHI
{

DescriptorSetLayout::DescriptorSetLayout(VkDevice device, VkDescriptorSetLayout handle)
    : device(device), handle(handle)
{
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    if (handle)
    {
        vkDestroyDescriptorSetLayout(device, handle, nullptr);
    }
}

} // namespace URay::RHI
