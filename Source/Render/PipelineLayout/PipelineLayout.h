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
    bool SupportsPushConstants() const { return isSupportingPushConstants; }
    void SetSupportPushConstants(bool value) { isSupportingPushConstants = value; }

    VkPipelineLayout GetHandle() const { return handle; }

private:
    VkDevice device = VK_NULL_HANDLE;
    VkPipelineLayout handle = VK_NULL_HANDLE;

    bool isSupportingPushConstants = false;
};

} // namespace URay
