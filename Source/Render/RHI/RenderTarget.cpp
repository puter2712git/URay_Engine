#include "RenderTarget.h"

#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureDesc.h"
#include "Render/RHI/Texture/TextureView.h"

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
        .usage = TextureUsage::DepthAttachment,
    };

    std::unique_ptr<Texture> newDepthTexture(renderDevice.CreateTexture(depthDesc));
    if (!newDepthTexture)
        return false;

    std::unique_ptr<TextureView> newDepthView(renderDevice.CreateTextureView(newDepthTexture.get()));
    if (!newDepthView)
        return false;

    colorTextureView = std::move(newColorView);
    colorTexture = std::move(newColorTexture);

    depthTextureView = std::move(newDepthView);
    depthTexture = std::move(newDepthTexture);

    extent = newExtent;

    return true;
}

} // namespace URay::Render
