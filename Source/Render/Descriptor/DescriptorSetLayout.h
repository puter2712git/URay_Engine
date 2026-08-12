#pragma once

#include <vulkan/vulkan.h>

namespace URay::RHI
{

class DescriptorSetLayout
{
public:
    DescriptorSetLayout(VkDevice device, VkDescriptorSetLayout handle);
    ~DescriptorSetLayout();

public:
    VkDescriptorSetLayout GetHandle() const { return handle; }

private:
    VkDevice device = VK_NULL_HANDLE;

    VkDescriptorSetLayout handle = VK_NULL_HANDLE;
};

} // namespace URay::RHI
