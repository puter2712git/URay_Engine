#include "SwapChain.h"

#include <Core/Log/LogSystem.h>
#include <Core/Math/Math.h>
#include <Core/Type/TypeAlias.h>
#include <Render/RHI/Device.h>
#include <Render/RHI/Surface.h>
#include <Render/RHI/Vulkan/VulkanSurfaceSupport.h>

#include <algorithm>
#include <limits>

namespace URay::Render
{

SwapChain::SwapChain(Device& device) : device(device) {}

bool SwapChain::Initialize(const SwapChainDesc& desc)
{
    return Recreate(desc);
}

void SwapChain::Finalize()
{
    for (VkSemaphore semaphore : renderFinishedSemaphores)
    {
        vkDestroySemaphore(device.GetVKDevice(), semaphore, nullptr);
    }
    renderFinishedSemaphores.clear();

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
    const QueueFamilyIndices& indices = device.GetQueueFamilyIndices();

    if (!indices.IsComplete())
        return false;

    if (!device.SupportsPresent(desc.surface))
    {
        URAY_LOG("[SwapChain] Present queue does not support the surface.");
        return false;
    }

    const auto support = Vulkan::QuerySurfaceSupport(device.GetPhysicalDevice(), desc.surface.GetSurface());

    if (!support)
        return false;

    if (support->formats.empty() || support->presentModes.empty())
    {
        URAY_LOG("[SwapChain] Surface has no usable format or present mode.");
        return false;
    }

    Finalize();

    VkSurfaceFormatKHR surfaceFormat = ChooseFormat(support->formats);
    VkPresentModeKHR presentMode = ChoosePresentMode(support->presentModes);
    VkExtent2D extent = ChooseExtent(support->capabilities, desc.extent);

    uint32 imageCount = support->capabilities.minImageCount + 1;

    if (support->capabilities.maxImageCount > 0 &&
        imageCount > support->capabilities.maxImageCount)
    {
        imageCount = support->capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = desc.surface.GetSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32 queueFamilyIndices[] = {
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

    createInfo.preTransform = support->capabilities.currentTransform;
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

    renderFinishedSemaphores.resize(images.size());

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (VkSemaphore& semaphore : renderFinishedSemaphores)
    {
        if (vkCreateSemaphore(device.GetVKDevice(), &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
            return false;
    }

    return true;
}

VkResult SwapChain::AcquireNextImage(VkSemaphore signalSemaphore, uint32& imageIndex)
{
    return vkAcquireNextImageKHR(
        device.GetVKDevice(),
        handle,
        UINT64_MAX,
        signalSemaphore,
        VK_NULL_HANDLE,
        &imageIndex);
}

VkResult SwapChain::Present(uint32 imageIndex, VkSemaphore waitSemaphore)
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

VkSemaphore SwapChain::GetRenderFinishedSemaphore(uint32 imageIndex) const
{
    return renderFinishedSemaphores.at(imageIndex);
}

VkImage SwapChain::GetImage(uint32 index) const
{
    if (index >= images.size())
        return VK_NULL_HANDLE;

    return images[index];
}

VkImageView SwapChain::GetImageView(uint32 index) const
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
    // TODO: Choose optimal present mode by OS or device.
    return VK_PRESENT_MODE_FIFO_KHR;

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
}

VkExtent2D SwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& requestedExtent) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = {};
        actualExtent.width = requestedExtent.width;
        actualExtent.height = requestedExtent.height;

        actualExtent.width = Math::Clamp(
            actualExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        actualExtent.height = Math::Clamp(
            actualExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

} // namespace URay::Render
