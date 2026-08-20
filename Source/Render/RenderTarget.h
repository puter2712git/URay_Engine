#pragma once

#include "Core/Math/Extent2D.h"

#include <memory>

namespace URay::RHI
{

class RenderDevice;
class Texture;
class TextureView;

class RenderTarget
{
public:
    RenderTarget(RenderDevice& renderDevice, const Extent2D& extent);
    ~RenderTarget();

public:
    bool Resize(const Extent2D& newExtent);

    TextureView* GetColorView() const { return colorTextureView.get(); }
    TextureView* GetDepthView() const { return depthTextureView.get(); }
    const Extent2D& GetExtent() const { return extent; }

private:
    RenderDevice& renderDevice;

    std::unique_ptr<Texture> colorTexture = nullptr;
    std::unique_ptr<TextureView> colorTextureView = nullptr;

    std::unique_ptr<Texture> depthTexture = nullptr;
    std::unique_ptr<TextureView> depthTextureView = nullptr;

    Extent2D extent = {};
};

} // namespace URay::RHI
