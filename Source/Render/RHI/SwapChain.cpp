#include "SwapChain.h"

#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Vulkan/VulkanContext.h"
#include "Render/RHI/Vulkan/VulkanSurfaceSupport.h"

#include <algorithm>
#include <limits>

namespace URay::Render
{

SwapChain::SwapChain(RenderDevice& device, VulkanContext& context)
    : device(device), context(context)
{
}

bool SwapChain::Initialize(const SwapChainDesc& desc)
{
    return Recreate(desc);
}

void SwapChain::Finalize()
{
    for (VkImageView view : imageViews)
    {
        vkDestroyImageView(device.GetVKDevice(), view, nullptr);
    }

    imageViews.clear();
    images.clear();

    if (handle)
    {
        vkDestroySwapchainKHR(device.GetVKDevice(), handle, nullptr);
        handle = VK_NULL_HANDLE;
    }
}

bool SwapChain::Recreate(const SwapChainDesc& desc)
{
    Finalize();

    Vulkan::SurfaceSupportDetails details =
        Vulkan::QuerySurfaceSupport(device.GetPhysicalDevice(), context.GetSurface());

    VkSurfaceFormatKHR surfaceFormat = ChooseFormat(details.formats);
    VkPresentModeKHR presentMode = ChoosePresentMode(details.presentModes);
    VkExtent2D extent = ChooseExtent(details.capabilities, desc.extent);

    uint32_t imageCount = details.capabilities.minImageCount + 1;

    if (details.capabilities.maxImageCount > 0 &&
        imageCount > details.capabilities.maxImageCount)
    {
        imageCount = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = context.GetSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device.FindQueueFamilyIndices(device.GetPhysicalDevice());
    uint32_t queueFamilyIndices[] = {
        indices.graphicsFamily.value(),
        indices.presentFamily.value(),
    };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = details.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device.GetVKDevice(), &createInfo, nullptr, &handle) != VK_SUCCESS)
        return false;

    vkGetSwapchainImagesKHR(device.GetVKDevice(), handle, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device.GetVKDevice(), handle, &imageCount, images.data());

    this->format = surfaceFormat.format;
    this->extent = extent;

    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); ++i)
    {
        imageViews[i] = device.CreateImageView(images[i], format, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    return true;
}

VkResult SwapChain::AcquireNextImage(VkSemaphore signalSemaphore, uint32_t& imageIndex)
{
    return vkAcquireNextImageKHR(
        device.GetVKDevice(),
        handle,
        UINT64_MAX,
        signalSemaphore,
        VK_NULL_HANDLE,
        &imageIndex);
}

VkResult SwapChain::Present(uint32_t imageIndex, VkSemaphore waitSemaphore)
{
    VkSwapchainKHR swapChains[] = { handle };

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &waitSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    return vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
}

VkImageView SwapChain::GetImageView(uint32_t index) const
{
    if (index >= imageViews.size())
        return VK_NULL_HANDLE;

    return imageViews[index];
}

VkSurfaceFormatKHR SwapChain::ChooseFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR SwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
{
    for (const auto& mode : availablePresentModes)
    {
        if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            return mode;
    }

    for (const auto& mode : availablePresentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& requestedExtent) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = {};
        actualExtent.width = requestedExtent.width;
        actualExtent.height = requestedExtent.height;

        actualExtent.width = std::clamp(
            actualExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(
            actualExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

} // namespace URay::Render
