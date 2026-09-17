#include "Render/Rendering/Renderer.h"

#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/CommandBuffer/CommandPool.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/Framebuffer.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/SwapChain.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureDesc.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RHI/Vulkan/VulkanContext.h"
#include "Render/RHI/Vulkan/VulkanUtils.h"
#include "Render/Rendering/RenderConstants.h"
#include "Render/Rendering/RenderInfo.h"
#include "Render/Rendering/RenderPass/RenderPass.h"
#include "Render/Rendering/Scene/RenderScene.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Type/Types.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"

#include "Platform/Window/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace URay::Render
{

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    renderer->framebufferResized = true;
}

Renderer::Renderer(Window& window, VulkanContext& context, RenderDevice& device, ResourceManager& resourceManager)
    : window(window), context(context), device(device), resourceManager(resourceManager) {}

Renderer::~Renderer() = default;

bool Renderer::Initialize()
{
    glfwSetWindowUserPointer(window.GetGLFWWindow(), this);
    glfwSetFramebufferSizeCallback(window.GetGLFWWindow(), FramebufferResizeCallback);

    int width, height;
    glfwGetFramebufferSize(window.GetGLFWWindow(), &width, &height);

    SwapChainDesc swapChainDesc = {};
    swapChainDesc.extent = {
        .width = static_cast<uint32>(width),
        .height = static_cast<uint32>(height),
    };

    swapChain.reset(device.CreateSwapChain(swapChainDesc));

    if (!CreateCommandPool())
        return false;

    if (!CreateSceneRenderTarget())
        return false;
    if (!CreatePostProcessRenderTarget())
        return false;

    if (!CreateFrameResources())
        return false;

    return true;
}

void Renderer::Finalize()
{
    vkDeviceWaitIdle(device.GetVKDevice());

    CleanupSwapChain();

    DestroyFrameResources();

    DestroySceneRenderTarget();
    DestroyPostProcessRenderTarget();

    DestroyCommandPool();
}

bool Renderer::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

    auto addFont = [&](const char* path, bool merge)
    {
        const std::vector<uint8> bytes = filesystem.ReadBinary(path);
        if (bytes.empty())
            return false;

        void* data = IM_ALLOC(bytes.size());
        std::memcpy(data, bytes.data(), bytes.size());

        ImFontConfig config;
        config.FontDataOwnedByAtlas = true;
        config.MergeMode = merge;

        if (!io.Fonts->AddFontFromMemoryTTF(
                data,
                static_cast<int>(bytes.size()),
                18.0f,
                &config))
        {
            IM_FREE(data);
            return false;
        }

        return true;
    };

    if (!addFont("Engine://Asset/Source/Font/NotoSansKR-Regular.ttf", false))
        return false;

    if (!addFont("Engine://Asset/Source/Font/NotoSansJP-Regular.ttf", true))
        return false;

    if (!addFont("Engine://Asset/Source/Font/NotoSansSC-Regular.ttf", true))
        return false;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window.GetGLFWWindow(), true);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.ApiVersion = VK_API_VERSION_1_3;
    initInfo.Instance = context.GetInstance();
    initInfo.PhysicalDevice = device.GetPhysicalDevice();
    initInfo.Device = device.GetVKDevice();
    initInfo.QueueFamily = device.FindQueueFamilyIndices(device.GetPhysicalDevice()).graphicsFamily.value();
    initInfo.Queue = device.GetGraphicsQueue();
    initInfo.DescriptorPool = VK_NULL_HANDLE;
    initInfo.DescriptorPoolSize = 8;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = static_cast<uint32>(swapChain->GetImageViews().size());

    const VkFormat colorFormat = swapChain->GetFormat();

    initInfo.UseDynamicRendering = true;
    initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &colorFormat;

    ImGui_ImplVulkan_Init(&initInfo);

    sceneImGuiTexture = ImGui_ImplVulkan_AddTexture(
        postProcessRenderTarget->GetColorView()->GetHandle(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    return true;
}

void Renderer::FinalizeImGui()
{
    if (sceneImGuiTexture != VK_NULL_HANDLE)
    {
        ImGui_ImplVulkan_RemoveTexture(sceneImGuiTexture);
        sceneImGuiTexture = VK_NULL_HANDLE;
    }

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool Renderer::BeginFrame()
{
    vkWaitForFences(device.GetVKDevice(), 1, &frameResources[currentFrame].inFlightFence, VK_TRUE, UINT64_MAX);

    ProcessPendingSceneRenderTargetResize();

    VkResult result = swapChain->AcquireNextImage(frameResources[currentFrame].imageAvailableSemaphore, imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || result == VK_NOT_READY)
    {
        RecreateSwapChain();
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image.");
    }

    vkResetFences(device.GetVKDevice(), 1, &frameResources[currentFrame].inFlightFence);

    if (!frameResources[currentFrame].commandBuffer->Reset())
        return false;

    if (!frameResources[currentFrame].commandBuffer->Begin(CommandBufferUsage::None))
        return false;

    return true;
}

void Renderer::EndFrame()
{
    if (!frameResources[currentFrame].commandBuffer->End())
        return;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkCommandBuffer vkCommandBuffer = frameResources[currentFrame].commandBuffer->GetHandle();

    VkSemaphore waitSemaphores[] = { frameResources[currentFrame].imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    VkSemaphore signalSemaphores[] = { swapChain->GetRenderFinishedSemaphore(imageIndex) };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, frameResources[currentFrame].inFlightFence) != VK_SUCCESS)
        return;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain->GetHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
        framebufferResized = false;
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image.");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::WaitIdle()
{
    vkDeviceWaitIdle(device.GetVKDevice());
}

void Renderer::RequestSceneRenderTargetResize(const Extent2D& extent)
{
    if (extent.width == 0 || extent.height == 0)
        return;

    const Extent2D currExtent = sceneRenderTarget->GetExtent();

    if (currExtent.width == extent.width && currExtent.height == extent.height)
    {
        pendingSceneRenderTargetExtent.reset();
        return;
    }

    pendingSceneRenderTargetExtent = extent;
}

Extent2D Renderer::GetSceneRenderTargetExtent() const
{
    return sceneRenderTarget->GetExtent();
}

VkImage Renderer::GetSwapChainImage() const
{
    return swapChain->GetImage(imageIndex);
}

VkImageView Renderer::GetSwapChainImageView() const
{
    return swapChain->GetImageView(imageIndex);
}

VkExtent2D Renderer::GetSwapChainExtent() const
{
    return swapChain->GetExtent();
}

void Renderer::CleanupSwapChain()
{
    swapChain->Finalize();
    swapChain.reset();
}

void Renderer::RecreateSwapChain()
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window.GetGLFWWindow(), &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window.GetGLFWWindow(), &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.GetVKDevice());

    CleanupSwapChain();

    SwapChainDesc swapChainDesc = {};
    swapChainDesc.extent = {
        static_cast<uint32>(width),
        static_cast<uint32>(height),
    };
    swapChain.reset(device.CreateSwapChain(swapChainDesc));
    if (!swapChain)
        return;
}

bool Renderer::CreateSceneRenderTarget()
{
    const VkExtent2D swapChainExtent = swapChain->GetExtent();
    const Extent2D extent = {
        .width = swapChainExtent.width,
        .height = swapChainExtent.height,
    };

    sceneRenderTarget = std::make_unique<RenderTarget>(device, extent);
    return true;
}

void Renderer::DestroySceneRenderTarget()
{
    if (sceneRenderTarget)
    {
        sceneRenderTarget.reset();
    }
}

bool Renderer::CreatePostProcessRenderTarget()
{
    const Extent2D extent = {
        .width = swapChain->GetExtent().width,
        .height = swapChain->GetExtent().height
    };

    postProcessRenderTarget = std::make_unique<RenderTarget>(device, extent);

    return postProcessRenderTarget != nullptr;
}

void Renderer::DestroyPostProcessRenderTarget()
{
    if (postProcessRenderTarget)
    {
        postProcessRenderTarget.reset();
    }
}

bool Renderer::CreateCommandPool()
{
    commandPool.reset(device.CreateCommandPool(QueueType::Graphics, CommandPoolFlags::ResetCommandBuffer));
    if (!commandPool)
        return false;

    return true;
}

void Renderer::DestroyCommandPool()
{
    if (commandPool)
    {
        commandPool.reset();
    }
}

bool Renderer::CreateFrameResources()
{
    DescriptorSetLayoutDesc description = device.MakeFrameDescriptorSetLayoutDescription();

    frameDescriptorSetLayout.reset(device.CreateDescriptorSetLayout(description));
    if (!frameDescriptorSetLayout)
        return false;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(
                device.GetVKDevice(), &semaphoreInfo, nullptr,
                &frameResources[i].imageAvailableSemaphore) != VK_SUCCESS)
        {
            return false;
        }
        if (vkCreateFence(
                device.GetVKDevice(), &fenceInfo, nullptr,
                &frameResources[i].inFlightFence) != VK_SUCCESS)
        {
            return false;
        }

        frameResources[i].commandBuffer.reset(commandPool->Allocate());

        UniformBufferDesc uniformBufferDesc = {};
        uniformBufferDesc.size = sizeof(FrameConstants);

        frameResources[i].uniformBuffer.reset(device.CreateUniformBuffer(uniformBufferDesc));

        StorageBufferDesc pointLightStorageBufferDesc = {
            .elementCapacity = 256,
            .elementStride = sizeof(PointLightConstants),
            .memoryUsage = MemoryUsage::CpuToGpu
        };
        frameResources[i].pointLightStorageBuffer.reset(device.CreateStorageBuffer(pointLightStorageBufferDesc));

        StorageBufferDesc spotLightStorageBufferDesc = {
            .elementCapacity = 256,
            .elementStride = sizeof(SpotLightConstants),
            .memoryUsage = MemoryUsage::CpuToGpu
        };
        frameResources[i].spotLightStorageBuffer.reset(device.CreateStorageBuffer(spotLightStorageBufferDesc));

        frameResources[i].descriptorSet.reset(device.CreateDescriptorSet(frameDescriptorSetLayout.get()));
        frameResources[i].descriptorSet->WriteUniformBuffer(0, *frameResources[i].uniformBuffer);
        frameResources[i].descriptorSet->WriteStorageBuffer(1, *frameResources[i].pointLightStorageBuffer);
        frameResources[i].descriptorSet->WriteStorageBuffer(2, *frameResources[i].spotLightStorageBuffer);
    }

    return true;
}

void Renderer::DestroyFrameResources()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        frameResources[i].descriptorSet.reset();

        frameResources[i].spotLightStorageBuffer.reset();
        frameResources[i].pointLightStorageBuffer.reset();
        frameResources[i].uniformBuffer.reset();

        frameResources[i].commandBuffer.reset();

        vkDestroyFence(
            device.GetVKDevice(),
            frameResources[i].inFlightFence, nullptr);
        vkDestroySemaphore(
            device.GetVKDevice(),
            frameResources[i].imageAvailableSemaphore, nullptr);
    }

    frameDescriptorSetLayout.reset();
}

void Renderer::ProcessPendingSceneRenderTargetResize()
{
    if (!pendingSceneRenderTargetExtent.has_value())
        return;

    const Extent2D extent = *pendingSceneRenderTargetExtent;
    pendingSceneRenderTargetExtent.reset();

    vkDeviceWaitIdle(device.GetVKDevice());

    if (sceneImGuiTexture != VK_NULL_HANDLE)
    {
        ImGui_ImplVulkan_RemoveTexture(sceneImGuiTexture);
        sceneImGuiTexture = VK_NULL_HANDLE;
    }

    if (!sceneRenderTarget->Resize(extent))
    {
        throw std::runtime_error("Failed to resize scene render target.");
    }
    if (!postProcessRenderTarget->Resize(extent))
    {
        throw std::runtime_error("Failed to resize post process render target.");
    }

    sceneImGuiTexture = ImGui_ImplVulkan_AddTexture(
        postProcessRenderTarget->GetColorView()->GetHandle(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Format Renderer::FindDepthFormat() const
{
    const VkFormat format = FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT,
          VK_FORMAT_D32_SFLOAT_S8_UINT,
          VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    return Vulkan::FromVkFormat(format);
}

bool Renderer::HasStencilComponent(VkFormat format) const
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat Renderer::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                       VkFormatFeatureFlags features) const
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device.GetPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    throw std::runtime_error("Failed to find supported format!");
}

} // namespace URay::Render
