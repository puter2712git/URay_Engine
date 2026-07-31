#pragma once

#include <vulkan/vulkan.h>

namespace URay
{

class PipelineLayout
{
public:
    PipelineLayout(VkDevice device, VkPipelineLayout handle);
    ~PipelineLayout();

public:
    VkPipelineLayout GetHandle() const { return handle; }

private:
    VkDevice device = VK_NULL_HANDLE;
    VkPipelineLayout handle = VK_NULL_HANDLE;
};

} // namespace URay
