#include "DescriptorSetLayout.h"

namespace URay::Render
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

} // namespace URay::Render
