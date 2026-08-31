#pragma once

#include <vulkan/vulkan.h>

#include <span>

namespace URay::Render
{

struct RenderPassBeginInfo
{
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;

    VkRect2D renderArea = {};

    std::span<const VkClearValue> clearValues;

    VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE;
};

} // namespace URay::Render
