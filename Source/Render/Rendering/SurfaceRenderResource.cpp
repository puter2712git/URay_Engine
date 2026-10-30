#include "SurfaceRenderResource.h"

#include <Platform/Window/Window.h>
#include <Render/RHI/Device.h>
#include <Render/RHI/SwapChain.h>

namespace URay::Render
{

SurfaceRenderResource::SurfaceRenderResource(Window& window, Surface& surface, Device& device)
    : window(window), surface(surface), device(device)
{
}

SurfaceRenderResource::~SurfaceRenderResource() = default;

bool SurfaceRenderResource::Initialize()
{
    window.Register("FramebufferResized", this, &SurfaceRenderResource::OnFramebufferResized);

    if (!RecreateSwapChain())
    {
        Finalize();
        return false;
    }

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (VkSemaphore& semaphore : imageAvailableSemaphores)
    {
        const VkResult result = vkCreateSemaphore(device.GetVKDevice(), &semaphoreInfo, nullptr, &semaphore);

        if (result != VK_SUCCESS)
        {
            Finalize();
            return false;
        }
    }

    return true;
}

void SurfaceRenderResource::Finalize()
{
    window.UnregisterAll(this);

    DestroySwapChain();

    for (VkSemaphore& semaphore : imageAvailableSemaphores)
    {
        if (semaphore == VK_NULL_HANDLE)
            continue;

        vkDestroySemaphore(device.GetVKDevice(), semaphore, nullptr);
        semaphore = VK_NULL_HANDLE;
    }

    imageIndex = 0;
    needsSwapChainRecreate = false;
}

VkResult SurfaceRenderResource::AcquireNextImage(uint32 frameIndex)
{
    if (!swapChain || frameIndex >= imageAvailableSemaphores.size())
        return VK_ERROR_INITIALIZATION_FAILED;

    return swapChain->AcquireNextImage(
        imageAvailableSemaphores[frameIndex],
        imageIndex);
}

VkSemaphore SurfaceRenderResource::GetImageAvailableSemaphore(uint32 frameIndex) const
{
    return imageAvailableSemaphores.at(frameIndex);
}

VkSemaphore SurfaceRenderResource::GetRenderFinishedSemaphore() const
{
    return swapChain->GetRenderFinishedSemaphore(imageIndex);
}

VkSwapchainKHR SurfaceRenderResource::GetSwapChainHandle() const
{
    return swapChain->GetHandle();
}

VkImage SurfaceRenderResource::GetImage() const
{
    return swapChain->GetImage(imageIndex);
}

VkImageView SurfaceRenderResource::GetImageView() const
{
    return swapChain->GetImageView(imageIndex);
}

VkExtent2D SurfaceRenderResource::GetExtent() const
{
    return swapChain->GetExtent();
}

Extent2D SurfaceRenderResource::GetFramebufferSize() const
{
    return window.GetFramebufferSize();
}

bool SurfaceRenderResource::RecreateSwapChain()
{
    const Extent2D framebufferSize = window.GetFramebufferSize();

    if (framebufferSize.width == 0 || framebufferSize.height == 0)
        return false;

    DestroySwapChain();

    const SwapChainDesc desc{
        .surface = surface,
        .extent = { framebufferSize.width,
                    framebufferSize.height }
    };

    swapChain.reset(device.CreateSwapChain(desc));

    if (!swapChain)
        return false;

    needsSwapChainRecreate = false;

    return true;
}

void SurfaceRenderResource::RequestSwapChainRecreate()
{
    needsSwapChainRecreate = true;
}

void SurfaceRenderResource::OnFramebufferResized(Extent2D extent)
{
    RequestSwapChainRecreate();
}

void SurfaceRenderResource::DestroySwapChain()
{
    if (!swapChain)
        return;

    swapChain->Finalize();
    swapChain.reset();
}

} // namespace URay::Render
