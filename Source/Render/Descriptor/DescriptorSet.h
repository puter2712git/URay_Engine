#pragma once

#include <vulkan/vulkan.h>

namespace URay
{

class DescriptorSetLayout;

class DescriptorSet
{
public:
    DescriptorSet(VkDevice device, VkDescriptorSet handle);
    ~DescriptorSet();

public:
    VkDescriptorSet GetHandle() const { return handle; }

    DescriptorSetLayout* GetLayout() const { return layout; }

private:
    VkDevice device = VK_NULL_HANDLE;
    VkDescriptorSet handle = VK_NULL_HANDLE;

    DescriptorSetLayout* layout = nullptr;
};

} // namespace URay
