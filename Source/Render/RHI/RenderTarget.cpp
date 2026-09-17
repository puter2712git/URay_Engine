#include "RenderTarget.h"

#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/CommandBuffer/ImageBarrier.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureDesc.h"
#include "Render/RHI/Texture/TextureView.h"

#include <array>
#include <stdexcept>

namespace URay::Render
{

RenderTarget::RenderTarget(RenderDevice& renderDevice, const RenderTargetDesc& desc)
    : device(renderDevice), desc(desc)
{
    if (!Recreate(desc.extent))
        throw std::runtime_error("Failed to create render target.");
}

RenderTarget::~RenderTarget() = default;

bool RenderTarget::Recreate(const Extent2D& newExtent)
{
    if (newExtent.width == 0 || newExtent.height == 0)
        return false;

    RenderTargetDesc newDesc = desc;
    newDesc.extent = newExtent;

    if (newDesc.color)
    {
        TextureDesc colorDesc = {};
        colorDesc.width = newExtent.width;
        colorDesc.height = newExtent.height;
        colorDesc.format = desc.color->format;
        colorDesc.usage = desc.color->usage;

        colorTexture.reset(device.CreateTexture(colorDesc));
        if (!colorTexture)
            return false;

        colorView.reset(device.CreateTextureView(colorTexture.get()));
        if (!colorView)
            return false;
    }

    if (newDesc.depth)
    {
        TextureDesc depthDesc = {};
        depthDesc.width = newExtent.width;
        depthDesc.height = newExtent.height;
        depthDesc.format = desc.depth->format;
        depthDesc.usage = desc.depth->usage;

        depthTexture.reset(device.CreateTexture(depthDesc));
        if (!depthTexture)
            return false;

        depthView.reset(device.CreateTextureView(depthTexture.get()));
        if (!depthView)
            return false;
    }

    desc = newDesc;
    colorLayout = ImageLayout::Undefined;
    depthLayout = ImageLayout::Undefined;

    return true;
}

void RenderTarget::TransitionColor(CommandBuffer& commandBuffer, ImageLayout newLayout)
{
    if (colorLayout == newLayout)
        return;

    const VkImageSubresourceRange range = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const SyncInfo src = GetSyncInfo(colorLayout);
    const SyncInfo dst = GetSyncInfo(newLayout);

    const std::array<ImageBarrierDesc, 1> barriers = {
        ImageBarrierDesc{
            .image = colorTexture->GetHandle(),
            .subresourceRange = range,
            .oldLayout = colorLayout,
            .newLayout = newLayout,
            .srcStage = src.stage,
            .srcAccess = src.access,
            .dstStage = dst.stage,
            .dstAccess = dst.access }
    };

    commandBuffer.PipelineBarrier(barriers);
    colorLayout = newLayout;
}

void RenderTarget::TransitionDepth(CommandBuffer& commandBuffer, ImageLayout newLayout)
{
    if (depthLayout == newLayout)
        return;

    const VkImageSubresourceRange range = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const SyncInfo src = GetSyncInfo(depthLayout);
    const SyncInfo dst = GetSyncInfo(newLayout);

    const std::array<ImageBarrierDesc, 1> barriers = {
        ImageBarrierDesc{
            .image = depthTexture->GetHandle(),
            .subresourceRange = range,
            .oldLayout = depthLayout,
            .newLayout = newLayout,
            .srcStage = src.stage,
            .srcAccess = src.access,
            .dstStage = dst.stage,
            .dstAccess = dst.access }
    };

    commandBuffer.PipelineBarrier(barriers);
    depthLayout = newLayout;
}

RenderTarget::SyncInfo RenderTarget::GetSyncInfo(ImageLayout layout)
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
