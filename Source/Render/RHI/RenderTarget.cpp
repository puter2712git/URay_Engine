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

RenderTarget::RenderTarget(RenderDevice& renderDevice, const Extent2D& extent)
    : renderDevice(renderDevice)
{
    if (!Resize(extent))
        throw std::runtime_error("Failed to create render target.");
}

RenderTarget::~RenderTarget() = default;

bool RenderTarget::Resize(const Extent2D& newExtent)
{
    if (newExtent.width == 0 || newExtent.height == 0)
        return false;

    const TextureDesc colorDesc = {
        .width = newExtent.width,
        .height = newExtent.height,
        .format = Format::BGRA8_sRGB,
        .usage = TextureUsage::ColorAttachment | TextureUsage::Sampled,
    };

    std::unique_ptr<Texture> newColorTexture(renderDevice.CreateTexture(colorDesc));
    if (!newColorTexture)
        return false;

    std::unique_ptr<TextureView> newColorView(renderDevice.CreateTextureView(newColorTexture.get()));
    if (!newColorView)
        return false;

    const TextureDesc depthDesc = {
        .width = newExtent.width,
        .height = newExtent.height,
        .format = Format::D32_Float_S8_UInt,
        .usage = TextureUsage::DepthAttachment | TextureUsage::Sampled,
    };

    std::unique_ptr<Texture> newDepthTexture(renderDevice.CreateTexture(depthDesc));
    if (!newDepthTexture)
        return false;

    std::unique_ptr<TextureView> newDepthView(renderDevice.CreateTextureView(newDepthTexture.get()));
    if (!newDepthView)
        return false;

    colorTextureView = std::move(newColorView);
    colorTexture = std::move(newColorTexture);
    colorLayout = ImageLayout::Undefined;

    depthTextureView = std::move(newDepthView);
    depthTexture = std::move(newDepthTexture);
    depthLayout = ImageLayout::Undefined;

    extent = newExtent;

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
