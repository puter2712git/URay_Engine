#pragma once

#include <vulkan/vulkan.h>

#include <memory>

namespace URay::RHI
{

class RenderDevice;
class Texture;
class TextureView;

class RenderTarget
{
public:
    RenderTarget(RenderDevice& renderDevice, VkExtent2D extent);
    ~RenderTarget();

public:
    bool Resize(VkExtent2D newExtent);

    TextureView* GetColorView() const { return colorTextureView.get(); }
    TextureView* GetDepthView() const { return depthTextureView.get(); }
    VkExtent2D GetExtent() const { return extent; }

private:
    RenderDevice& renderDevice;

    std::unique_ptr<Texture> colorTexture = nullptr;
    std::unique_ptr<TextureView> colorTextureView = nullptr;

    std::unique_ptr<Texture> depthTexture = nullptr;
    std::unique_ptr<TextureView> depthTextureView = nullptr;

    VkExtent2D extent = {};
};

} // namespace URay::RHI
