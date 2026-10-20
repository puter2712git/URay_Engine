#pragma once

#include <vulkan/vulkan.h>

namespace URay::Render
{

struct BufferBarrierDesc
{
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceSize offset = 0;
    VkDeviceSize size = VK_WHOLE_SIZE;

    VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 srcAccess = VK_ACCESS_2_NONE;
    VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 dstAccess = VK_ACCESS_2_NONE;
};

} // namespace URay::Render
