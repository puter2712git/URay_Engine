#pragma once

#include "Render/RHI/Texture/TextureDesc.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

struct ImageBarrierDesc
{
    VkImage image = VK_NULL_HANDLE;
    VkImageSubresourceRange subresourceRange = {};

    ImageLayout oldLayout = ImageLayout::Undefined;
    ImageLayout newLayout = ImageLayout::Undefined;

    VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 srcAccess = VK_ACCESS_2_NONE;
    VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 dstAccess = VK_ACCESS_2_NONE;
};

} // namespace URay::Render
