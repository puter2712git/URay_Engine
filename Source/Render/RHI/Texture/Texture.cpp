#include "Texture.h"

#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/CommandBuffer/ImageBarrier.h"
#include "Render/RHI/RenderDevice.h"

namespace URay::Render
{

Texture::Texture(RenderDevice& device, VkImage handle, VkDeviceMemory memory, const TextureDesc& desc)
    : device(device), handle(handle), memory(memory), desc(desc)
{
}

Texture::~Texture()
{
    vkDestroyImage(device.GetVKDevice(), handle, nullptr);
    vkFreeMemory(device.GetVKDevice(), memory, nullptr);
}

void Texture::Transition(CommandBuffer& commandBuffer, ImageLayout newLayout)
{
    if (imageLayout == newLayout)
        return;

    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_NONE;

    if (desc.format == Format::D32_Float)
    {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    else if (desc.format == Format::D32_Float_S8_UInt ||
             desc.format == Format::D24_UNorm_S8_UInt)
    {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
    {
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    const VkImageSubresourceRange range = {
        .aspectMask = aspectMask,
        .baseMipLevel = 0,
        .levelCount = desc.mipLevels,
        .baseArrayLayer = 0,
        .layerCount = desc.arrayLayers
    };

    const SyncInfo src = GetSyncInfo(imageLayout);
    const SyncInfo dst = GetSyncInfo(newLayout);

    const std::array<ImageBarrierDesc, 1> barriers = {
        ImageBarrierDesc{
            .image = handle,
            .subresourceRange = range,
            .oldLayout = imageLayout,
            .newLayout = newLayout,
            .srcStage = src.stage,
            .srcAccess = src.access,
            .dstStage = dst.stage,
            .dstAccess = dst.access }
    };

    commandBuffer.PipelineBarrier(barriers);
    imageLayout = newLayout;
}

Texture::SyncInfo Texture::GetSyncInfo(ImageLayout layout)
{
    switch (layout)
    {
    case ImageLayout::Undefined:
        return SyncInfo{
            .stage = VK_PIPELINE_STAGE_2_NONE,
            .access = VK_ACCESS_2_NONE
        };

    case ImageLayout::ColorAttachment:
        return SyncInfo{
            .stage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .access = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT
        };

    case ImageLayout::DepthAttachment:
        return SyncInfo{
            .stage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                     VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            .access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
        };

    case ImageLayout::ShaderReadOnly:
    case ImageLayout::DepthReadOnly:
        return SyncInfo{
            .stage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            .access = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT
        };

    default:
        return SyncInfo{
            .stage = VK_PIPELINE_STAGE_2_NONE,
            .access = VK_ACCESS_2_NONE
        };
    }
}

} // namespace URay::Render
