#include "RenderTarget.h"

#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/CommandBuffer/ImageBarrier.h"
#include "Render/RHI/Device.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureView.h"

#include <array>
#include <stdexcept>

namespace URay::Render
{

RenderTarget::RenderTarget(Device& device, const RenderTargetDesc& desc)
    : device(device), desc(desc)
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

    std::vector<std::unique_ptr<Texture>> newColorTextures;
    std::vector<std::unique_ptr<TextureView>> newColorViews;

    newColorTextures.reserve(newDesc.colorAttachments.size());
    newColorViews.reserve(newDesc.colorAttachments.size());

    for (const RenderTargetAttachmentDesc& attachment : newDesc.colorAttachments)
    {
        TextureDesc textureDesc = {};
        textureDesc.width = newExtent.width;
        textureDesc.height = newExtent.height;
        textureDesc.format = attachment.format;
        textureDesc.usage = attachment.usage;

        std::unique_ptr<Texture> texture(device.CreateTexture(textureDesc));
        if (!texture)
            return false;

        std::unique_ptr<TextureView> view(device.CreateTextureView(texture.get(), TextureViewDesc{}));
        if (!view)
            return false;

        newColorTextures.push_back(std::move(texture));
        newColorViews.push_back(std::move(view));
    }

    std::unique_ptr<Texture> newDepthTexture = nullptr;
    std::unique_ptr<TextureView> newDepthView = nullptr;

    if (newDesc.depth)
    {
        TextureDesc textureDesc = {};
        textureDesc.width = newExtent.width;
        textureDesc.height = newExtent.height;
        textureDesc.format = desc.depth->format;
        textureDesc.usage = desc.depth->usage;

        newDepthTexture.reset(device.CreateTexture(textureDesc));
        if (!newDepthTexture)
            return false;

        newDepthView.reset(device.CreateTextureView(newDepthTexture.get(), TextureViewDesc{}));
        if (!newDepthView)
            return false;
    }

    colorTextures.swap(newColorTextures);
    colorViews.swap(newColorViews);
    depthTexture.swap(newDepthTexture);
    depthView.swap(newDepthView);
    desc = std::move(newDesc);

    return true;
}

void RenderTarget::TransitionColor(CommandBuffer& commandBuffer, uint32 index, ImageLayout newLayout)
{
    Texture* texture = GetColorTexture(index);
    if (!texture)
        return;

    texture->Transition(commandBuffer, newLayout);
}

void RenderTarget::TransitionDepth(CommandBuffer& commandBuffer, ImageLayout newLayout)
{
    depthTexture->Transition(commandBuffer, newLayout);
}

Texture* RenderTarget::GetColorTexture(uint32 index) const
{
    if (index >= colorTextures.size())
        return nullptr;

    return colorTextures[index].get();
}

TextureView* RenderTarget::GetColorView(uint32 index) const
{
    if (index >= colorViews.size())
        return nullptr;

    return colorViews[index].get();
}

} // namespace URay::Render
