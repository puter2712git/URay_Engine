#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/RHI/Texture/Texture.h>

#include <Core/Math/Extent2D.h>

#include <vulkan/vulkan.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay::Render
{

class Device;
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
    Vector<RenderTargetAttachmentDesc> colorAttachments;
    std::optional<RenderTargetAttachmentDesc> depth;
};

class RenderTarget
{
public:
    RenderTarget(Device& device, const RenderTargetDesc& desc);
    ~RenderTarget();

public:
    bool Recreate(const Extent2D& newExtent);

    void TransitionColor(CommandBuffer& commandBuffer, uint32 index, ImageLayout newLayout);
    void TransitionDepth(CommandBuffer& commandBuffer, ImageLayout newLayout);

    Texture* GetColorTexture(uint32 index) const;
    TextureView* GetColorView(uint32 index) const;

    Texture* GetDepthTexture() const { return depthTexture.get(); }
    TextureView* GetDepthView() const { return depthView.get(); }

    const Extent2D& GetExtent() const { return desc.extent; }

private:
    Device& device;

    RenderTargetDesc desc = {};

    Vector<UniquePtr<Texture>> colorTextures;
    Vector<UniquePtr<TextureView>> colorViews;

    UniquePtr<Texture> depthTexture = nullptr;
    UniquePtr<TextureView> depthView = nullptr;
};

} // namespace URay::Render
