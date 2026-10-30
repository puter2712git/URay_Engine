#pragma once

#include <Core/Math/Extent2D.h>
#include <Core/Type/TypeAlias.h>
#include <Render/Rendering/RenderInfo.h>

#include <array>
#include <memory>

#include <vulkan/vulkan.h>

namespace URay
{
class Window;
}

namespace URay::Render
{

class Surface;
class SwapChain;
class Device;

class SurfaceRenderResource
{
public:
    SurfaceRenderResource(Window& window, Surface& surface, Device& device);
    ~SurfaceRenderResource();

public:
    bool Initialize();
    void Finalize();

    VkResult AcquireNextImage(uint32 frameIndex);

    VkSemaphore GetImageAvailableSemaphore(uint32 frameIndex) const;
    VkSemaphore GetRenderFinishedSemaphore() const;

    VkSwapchainKHR GetSwapChainHandle() const;
    VkImage GetImage() const;
    VkImageView GetImageView() const;
    VkExtent2D GetExtent() const;
    Extent2D GetFramebufferSize() const;
    uint32 GetImageIndex() const { return imageIndex; }
    VkFormat GetFormat() const;

    bool NeedsSwapChainRecreate() const { return needsSwapChainRecreate; }
    void ClearFramebufferResized() { needsSwapChainRecreate = false; }

    bool RecreateSwapChain();
    void RequestSwapChainRecreate();

private:
    void OnFramebufferResized(Extent2D extent);
    void DestroySwapChain();

private:
    Window& window;
    Surface& surface;
    Device& device;

    std::unique_ptr<SwapChain> swapChain;

    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores = {};

    uint32 imageIndex = 0;
    bool needsSwapChainRecreate = false;
};

} // namespace URay::Render
