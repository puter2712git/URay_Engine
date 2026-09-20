#include "RenderTarget.h"

#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/CommandBuffer/ImageBarrier.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Texture/Texture.h"
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

        colorView.reset(device.CreateTextureView(colorTexture.get(), TextureViewDesc{}));
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

        depthView.reset(device.CreateTextureView(depthTexture.get(), TextureViewDesc{}));
        if (!depthView)
            return false;
    }

    desc = newDesc;

    return true;
}

void RenderTarget::TransitionColor(CommandBuffer& commandBuffer, ImageLayout newLayout)
{
    colorTexture->Transition(commandBuffer, newLayout);
}

void RenderTarget::TransitionDepth(CommandBuffer& commandBuffer, ImageLayout newLayout)
{
    depthTexture->Transition(commandBuffer, newLayout);
}

} // namespace URay::Render
