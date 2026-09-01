#pragma once

#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace URay::Render
{

class RenderDevice;
class VulkanContext;

struct SwapChainDesc
{
    VkExtent2D extent = {};
};

class SwapChain
{
public:
    SwapChain(RenderDevice& device, VulkanContext& context);

public:
    bool Initialize(const SwapChainDesc& desc);
    void Finalize();

    bool Recreate(const SwapChainDesc& desc);

    VkResult AcquireNextImage(VkSemaphore signalSemaphore, uint32& imageIndex);
    VkResult Present(uint32 imageIndex, VkSemaphore waitSemaphore);

    VkSwapchainKHR GetHandle() const { return handle; }

    VkExtent2D GetExtent() const { return extent; }
    VkFormat GetFormat() const { return format; }
    VkImageView GetImageView(uint32 index) const;
    const std::vector<VkImageView>& GetImageViews() const { return imageViews; }

private:
    VkSurfaceFormatKHR ChooseFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& requestedExtent) const;

private:
    RenderDevice& device;
    VulkanContext& context;

    VkSwapchainKHR handle = VK_NULL_HANDLE;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

    VkExtent2D extent = {};
    VkFormat format = VK_FORMAT_UNDEFINED;
};

} // namespace URay::Render
