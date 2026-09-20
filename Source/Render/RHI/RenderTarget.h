#pragma once

#include "Render/RHI/Texture/Texture.h"

#include "Core/Math/Extent2D.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <optional>

namespace URay::Render
{

class RenderDevice;
class CommandBuffer;
class Texture;
class TextureView;

struct RenderTargetAttachmentDesc
{
    Format format = Format::Unknown;
    TextureUsage usage = TextureUsage::None;
};

struct RenderTargetDesc
{
    Extent2D extent = {};
    std::optional<RenderTargetAttachmentDesc> color;
    std::optional<RenderTargetAttachmentDesc> depth;
};

class RenderTarget
{
public:
    RenderTarget(RenderDevice& renderDevice, const RenderTargetDesc& desc);
    ~RenderTarget();

public:
    bool Recreate(const Extent2D& newExtent);

    void TransitionColor(CommandBuffer& commandBuffer, ImageLayout newLayout);
    void TransitionDepth(CommandBuffer& commandBuffer, ImageLayout newLayout);

    Texture* GetColorTexture() const { return colorTexture.get(); }
    TextureView* GetColorView() const { return colorView.get(); }

    Texture* GetDepthTexture() const { return depthTexture.get(); }
    TextureView* GetDepthView() const { return depthView.get(); }

    const Extent2D& GetExtent() const { return desc.extent; }

private:
    RenderDevice& device;

    RenderTargetDesc desc = {};

    std::unique_ptr<Texture> colorTexture = nullptr;
    std::unique_ptr<TextureView> colorView = nullptr;

    std::unique_ptr<Texture> depthTexture = nullptr;
    std::unique_ptr<TextureView> depthView = nullptr;
};

} // namespace URay::Render
