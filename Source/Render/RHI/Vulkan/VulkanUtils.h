#pragma once

#include "Render/RHI/Buffer/BufferUsage.h"
#include "Render/RHI/Attachment/RenderingInfo.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{
struct RenderingAttachmentInfo;
}

namespace URay::Render::Vulkan
{

VkFormat ToVkFormat(Format format);
Format FromVkFormat(VkFormat format);

VkBufferUsageFlags ToVkBufferUsage(BufferBindFlags flags);
VkMemoryPropertyFlags ToVkMemoryProperties(MemoryUsage usage);

VkImageUsageFlags ToVkImageUsageFlags(TextureUsage usage);
VkImageAspectFlags ToVkImageAspectFlags(Format format);

VkImageLayout ToVkImageLayout(ImageLayout layout);

VkAttachmentLoadOp ToVkLoadOp(LoadOp op);
VkAttachmentStoreOp ToVkStoreOp(StoreOp op);

VkRenderingAttachmentInfo ToVkAttachment(const RenderingAttachmentInfo& attachmentInfo);

} // namespace URay::Render::Vulkan
