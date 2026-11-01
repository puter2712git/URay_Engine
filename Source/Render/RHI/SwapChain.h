#pragma once

#include <Core/Type/TypeAlias.h>

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace URay::Render
{

class Device;
class Surface;

struct SwapChainDesc
{
    Surface& surface;
    VkExtent2D extent = {};
};

class SwapChain
{
public:
    SwapChain(Device& device);

public:
    bool Initialize(const SwapChainDesc& desc);
    void Finalize();

    bool Recreate(const SwapChainDesc& desc);

    VkResult AcquireNextImage(VkSemaphore signalSemaphore, uint32& imageIndex);
    VkResult Present(uint32 imageIndex, VkSemaphore waitSemaphore);

    VkSwapchainKHR GetHandle() const { return handle; }

    VkExtent2D GetExtent() const { return extent; }
    VkFormat GetFormat() const { return format; }
    VkImage GetImage(uint32 index) const;
    VkImageView GetImageView(uint32 index) const;
    const Vector<VkImageView>& GetImageViews() const { return imageViews; }

    VkSemaphore GetRenderFinishedSemaphore(uint32 imageInex) const;

private:
    VkSurfaceFormatKHR ChooseFormat(const Vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR ChoosePresentMode(const Vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& requestedExtent) const;

private:
    Device& device;

    VkSwapchainKHR handle = VK_NULL_HANDLE;
    Vector<VkImage> images;
    Vector<VkImageView> imageViews;

    VkExtent2D extent = {};
    VkFormat format = VK_FORMAT_UNDEFINED;

    Vector<VkSemaphore> renderFinishedSemaphores;
};

} // namespace URay::Render
