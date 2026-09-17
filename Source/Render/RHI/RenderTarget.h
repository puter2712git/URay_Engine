#pragma once

#include "Render/RHI/Texture/TextureDesc.h"

#include "Core/Math/Extent2D.h"

#include <vulkan/vulkan.h>

#include <memory>

namespace URay::Render
{

class RenderDevice;
class CommandBuffer;
class Texture;
class TextureView;

class RenderTarget
{
public:
    RenderTarget(RenderDevice& renderDevice, const Extent2D& extent);
    ~RenderTarget();

private:
    struct SyncInfo
    {
        VkPipelineStageFlags2 stage;
        VkAccessFlags2 access;
    };

public:
    bool Resize(const Extent2D& newExtent);

    void TransitionColor(CommandBuffer& commandBuffer, ImageLayout newLayout);
    void TransitionDepth(CommandBuffer& commandBuffer, ImageLayout newLayout);

    Texture* GetColorTexture() const { return colorTexture.get(); }
    TextureView* GetColorView() const { return colorTextureView.get(); }

    Texture* GetDepthTexture() const { return depthTexture.get(); }
    TextureView* GetDepthView() const { return depthTextureView.get(); }
    const Extent2D& GetExtent() const { return extent; }

private:
    SyncInfo GetSyncInfo(ImageLayout layout);

private:
    RenderDevice& renderDevice;

    std::unique_ptr<Texture> colorTexture = nullptr;
    std::unique_ptr<TextureView> colorTextureView = nullptr;
    ImageLayout colorLayout = ImageLayout::Undefined;

    std::unique_ptr<Texture> depthTexture = nullptr;
    std::unique_ptr<TextureView> depthTextureView = nullptr;
    ImageLayout depthLayout = ImageLayout::Undefined;

    Extent2D extent = {};
};

} // namespace URay::Render
