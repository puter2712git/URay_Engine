#include <Render/Rendering/Renderer.h>

#include <Render/RHI/Buffer/Buffer.h>
#include <Render/RHI/Buffer/BufferDesc.h>
#include <Render/RHI/CommandBuffer/CommandBuffer.h>
#include <Render/RHI/CommandBuffer/CommandPool.h>
#include <Render/RHI/Descriptor/DescriptorSet.h>
#include <Render/RHI/Descriptor/DescriptorSetLayout.h>
#include <Render/RHI/Device.h>
#include <Render/RHI/Framebuffer.h>
#include <Render/RHI/PipelineLayout/PipelineLayout.h>
#include <Render/RHI/PipelineState/PipelineState.h>
#include <Render/RHI/RenderTarget.h>
#include <Render/RHI/SwapChain.h>
#include <Render/RHI/Texture/Texture.h>
#include <Render/RHI/Texture/TextureView.h>
#include <Render/RHI/Vulkan/VulkanContext.h>
#include <Render/RHI/Vulkan/VulkanUtils.h>
#include <Render/Rendering/RenderConstants.h>
#include <Render/Rendering/RenderInfo.h>
#include <Render/Rendering/RenderPass/RenderPass.h>
#include <Render/Rendering/RenderTargetRegistry.h>
#include <Render/Rendering/Scene/RenderScene.h>
#include <Render/Rendering/Shadow/ShadowSystem.h>
#include <Render/ResourceManager.h>
#include <Render/Shader/Shader.h>

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Core/Performance/PerformanceViewModel.h>
#include <Core/Type/Types.h>

#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Scene.h>

#include <Platform/Window/Window.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace URay::Render
{

static std::string_view GetRenderPassName(RenderPassId passId)
{
    switch (passId)
    {
    case RenderPassId::Shadow: return "Shadow";
    case RenderPassId::Opaque: return "Opaque";
    case RenderPassId::SelectionMask: return "Selection Mask";
    case RenderPassId::Decal: return "Decal";
    case RenderPassId::Overlay: return "Overlay";
    case RenderPassId::Gizmo: return "Gizmo";
    case RenderPassId::Fog: return "Fog";
    case RenderPassId::SelectionOutline: return "Selection Outline";
    case RenderPassId::UI: return "UI";
    case RenderPassId::Count: break;
    }

    return "Unknown";
}

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    renderer->framebufferResized = true;
}

Renderer::Renderer(Window& window, VulkanContext& context, Device& device, ResourceManager& resourceManager)
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
    if (!swapChain)
    {
        URAY_LOG("[Renderer] Failed to create swap chain.");
        return false;
    }

    if (!CreateCommandPool())
    {
        URAY_LOG("[Renderer] Failed to create command pool.");
        Finalize();
        return false;
    }

    renderTargetRegistry = std::make_unique<RenderTargetRegistry>(device);

    if (!CreateSceneRenderTarget())
    {
        URAY_LOG("[Renderer] Failed to create scene render target.");
        Finalize();
        return false;
    }
    if (!CreatePostProcessRenderTarget())
    {
        URAY_LOG("[Renderer] Failed to create post process render target.");
        Finalize();
        return false;
    }
    if (!CreateSelectionMaskRenderTarget())
    {
        URAY_LOG("[Renderer] Failed to create selection mask render target.");
        Finalize();
        return false;
    }
    if (!CreateGizmoDepthRenderTarget())
    {
        URAY_LOG("[Renderer] Failed to create gizmo depth render target.");
        Finalize();
        return false;
    }

    if (!CreateFrameResources())
    {
        URAY_LOG("[Renderer] Failed to create frame resources.");
        Finalize();
        return false;
    }

    objectIdReadback = std::make_unique<ObjectIdReadback>(device);
    if (!objectIdReadback->Initialize())
    {
        URAY_LOG("[Renderer] Failed to initialize object ID readback.");
        Finalize();
        return false;
    }

    return true;
}

void Renderer::Finalize()
{
    vkDeviceWaitIdle(device.GetVKDevice());

    CleanupSwapChain();

    if (objectIdReadback)
    {
        objectIdReadback->Finalize();
        objectIdReadback.reset();
    }

    DestroyFrameResources();

    if (renderTargetRegistry)
    {
        renderTargetRegistry->DestroyAll();
        renderTargetRegistry.reset();
    }

    DestroyCommandPool();
}

bool Renderer::BeginFrame()
{
    URAY_PROFILE_SCOPE("Renderer::BeginFrame");

    FrameResource& frameResource = frameResources[currentFrame];

    {
        URAY_PROFILE_SCOPE("Renderer::WaitForInFlightFence");
        vkWaitForFences(device.GetVKDevice(), 1, &frameResources[currentFrame].inFlightFence, VK_TRUE, UINT64_MAX);
    }

    if (frameResource.gpuTimeStamps.hasPendingResults)
    {
        if (!ConsumeGPUTimestampResults(frameResource))
            return false;
    }

    {
        URAY_PROFILE_SCOPE("Renderer::ConsumeObjectIdReadback");

        if (const auto result = objectIdReadback->ConsumeCompleted(currentFrame))
        {
            objectIdReadbackRay.Emit(*result);
        }
    }

    {
        URAY_PROFILE_SCOPE("Renderer::ProcessPendingResize");
        ProcessPendingSceneRenderTargetResize();
    }

    VkResult result = VK_SUCCESS;
    {
        URAY_PROFILE_SCOPE("Renderer::AcquireSwapChainImage");
        result = swapChain->AcquireNextImage(frameResources[currentFrame].imageAvailableSemaphore, imageIndex);
    }

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || result == VK_NOT_READY)
    {
        RecreateSwapChain();
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image.");
    }

    {
        URAY_PROFILE_SCOPE("Renderer::ResetInFlightFence");
        vkResetFences(device.GetVKDevice(), 1, &frameResources[currentFrame].inFlightFence);
    }

    {
        URAY_PROFILE_SCOPE("Renderer::ResetCommandBuffer");

        if (!frameResources[currentFrame].commandBuffer->Reset())
            return false;
    }

    {
        URAY_PROFILE_SCOPE("Renderer::BeginCommandBuffer");

        if (!frameResource.commandBuffer->Begin(CommandBufferUsage::None))
            return false;

        vkCmdResetQueryPool(
            frameResource.commandBuffer->GetHandle(),
            frameResource.gpuTimeStamps.queryPool,
            0,
            GpuTimestampQueryCount);
    }

    return true;
}

void Renderer::EndFrame()
{
    URAY_PROFILE_SCOPE("Renderer::EndFrame");

    FrameResource& frameResource = frameResources[currentFrame];
    CommandBuffer& commandBuffer = *frameResource.commandBuffer;

    {
        URAY_PROFILE_SCOPE("Renderer::EndCommandBuffer");

        if (!commandBuffer.End())
            return;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkCommandBuffer vkCommandBuffer = commandBuffer.GetHandle();

    VkSemaphore waitSemaphores[] = { frameResource.imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    VkSemaphore signalSemaphores[] = { swapChain->GetRenderFinishedSemaphore(imageIndex) };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    {
        URAY_PROFILE_SCOPE("Renderer::QueueSubmit");

        if (vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, frameResource.inFlightFence) != VK_SUCCESS)
            return;
    }

    frameResource.gpuTimeStamps.hasPendingResults = true;
    frameResource.gpuTimeStamps.submissionFrameNumber = ++gpuTimestampSubmissionSequence;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain->GetHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = VK_SUCCESS;
    {
        URAY_PROFILE_SCOPE("Renderer::QueuePresent");
        result = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
    }

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

    Scheduler& scheduler = gEngine->GetScheduler();

    RenderTarget& sceneRT = renderTargetRegistry->Get(RenderTargetId::Scene);

    const Extent2D currExtent = GetSceneRenderTargetExtent();
    if (currExtent.width == extent.width && currExtent.height == extent.height)
    {
        if (sceneResizeTaskId)
        {
            scheduler.Cancel(*sceneResizeTaskId);
            sceneResizeTaskId.reset();
        }
        return;
    }

    if (sceneResizeTaskId &&
        scheduledSceneResizeExtent->width == extent.width &&
        scheduledSceneResizeExtent->height == extent.height)
    {
        return;
    }

    if (sceneResizeTaskId)
    {
        scheduler.Cancel(*sceneResizeTaskId);
    }

    scheduledSceneResizeExtent = extent;
    sceneResizeTaskId = scheduler.ScheduleOnce(
        SceneResizeDebounceSeconds,
        [this, extent]()
        {
            pendingSceneRenderTargetExtent = extent;
            scheduledSceneResizeExtent.reset();
            sceneResizeTaskId.reset();
        });
}

Extent2D Renderer::GetSceneRenderTargetExtent() const
{
    RenderTarget& sceneRT = renderTargetRegistry->Get(RenderTargetId::Scene);
    return sceneRT.GetExtent();
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
    if (swapChain)
    {
        swapChain->Finalize();
        swapChain.reset();
    }
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
    if (!renderTargetRegistry)
        return false;

    const VkExtent2D swapChainExtent = swapChain->GetExtent();

    std::vector<RenderTargetAttachmentDesc> colorAttachments;
    colorAttachments.push_back(RenderTargetAttachmentDesc{
        .format = Format::BGRA8_sRGB,
        .usage = TextureUsage::ColorAttachment | TextureUsage::Sampled });
    colorAttachments.push_back(RenderTargetAttachmentDesc{
        .format = Format::R32_UInt,
        .usage = TextureUsage::ColorAttachment | TextureUsage::TransferSrc });

    RenderTargetDesc desc = {};
    desc.extent.width = swapChainExtent.width;
    desc.extent.height = swapChainExtent.height;
    desc.colorAttachments = colorAttachments;
    desc.depth = RenderTargetAttachmentDesc{
        .format = Format::D32_Float_S8_UInt,
        .usage = TextureUsage::DepthAttachment | TextureUsage::Sampled
    };

    return renderTargetRegistry->Create(RenderTargetId::Scene, desc, true);
}

bool Renderer::CreatePostProcessRenderTarget()
{
    const VkExtent2D swapChainExtent = swapChain->GetExtent();

    std::vector<RenderTargetAttachmentDesc> colorAttachments;
    colorAttachments.push_back(RenderTargetAttachmentDesc{
        .format = Format::BGRA8_sRGB,
        .usage = TextureUsage::ColorAttachment | TextureUsage::Sampled });

    RenderTargetDesc desc = {};
    desc.extent.width = swapChainExtent.width;
    desc.extent.height = swapChainExtent.height;
    desc.colorAttachments = colorAttachments;
    desc.depth = RenderTargetAttachmentDesc{
        .format = Format::D32_Float_S8_UInt,
        .usage = TextureUsage::DepthAttachment | TextureUsage::Sampled
    };

    return renderTargetRegistry->Create(RenderTargetId::PostProcess, desc, true);
}

bool Renderer::CreateSelectionMaskRenderTarget()
{
    const VkExtent2D swapChainExtent = swapChain->GetExtent();

    std::vector<RenderTargetAttachmentDesc> colorAttachments;
    colorAttachments.push_back(RenderTargetAttachmentDesc{
        .format = Format::BGRA8_sRGB,
        .usage = TextureUsage::ColorAttachment | TextureUsage::Sampled });

    RenderTargetDesc desc = {};
    desc.extent.width = swapChainExtent.width;
    desc.extent.height = swapChainExtent.height;
    desc.colorAttachments = colorAttachments;
    desc.depth = RenderTargetAttachmentDesc{
        .format = Format::D32_Float_S8_UInt,
        .usage = TextureUsage::DepthAttachment | TextureUsage::Sampled
    };

    return renderTargetRegistry->Create(RenderTargetId::SelectionMask, desc, true);
}

bool Renderer::CreateGizmoDepthRenderTarget()
{
    const VkExtent2D swapChainExtent = swapChain->GetExtent();

    RenderTargetDesc desc = {};
    desc.extent.width = swapChainExtent.width;
    desc.extent.height = swapChainExtent.height;
    desc.depth = RenderTargetAttachmentDesc{
        .format = Format::D32_Float_S8_UInt,
        .usage = TextureUsage::DepthAttachment
    };

    return renderTargetRegistry->Create(RenderTargetId::GizmoDepth, desc, true);
}

bool Renderer::CreateCommandPool()
{
    commandPool.reset(device.CreateCommandPool(QueueType::Graphics, CommandPoolFlags::ResetCommandBuffer));
    if (!commandPool)
    {
        URAY_LOG("[Renderer] Failed to create graphics command pool.");
        return false;
    }

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

    Sampler* shadowSampler = resourceManager.GetOrCreateSampler({});
    if (!shadowSampler)
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

        VkQueryPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        poolCreateInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
        poolCreateInfo.queryCount = GpuTimestampQueryCount;

        if (vkCreateQueryPool(device.GetVKDevice(), &poolCreateInfo, nullptr,
                              &frameResources[i].gpuTimeStamps.queryPool) != VK_SUCCESS)
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

        UniformBufferDesc shadowUniformBufferDesc = {};
        shadowUniformBufferDesc.size = sizeof(ShadowConstants);

        frameResources[i].shadowUniformBuffer.reset(device.CreateUniformBuffer(shadowUniformBufferDesc));

        StorageBufferDesc spotLightShadowStorageBufferDesc = {
            .elementCapacity = 256,
            .elementStride = sizeof(SpotLightShadowConstants),
            .memoryUsage = MemoryUsage::CpuToGpu
        };
        frameResources[i].spotLightShadowStorageBuffer.reset(device.CreateStorageBuffer(spotLightShadowStorageBufferDesc));

        StorageBufferDesc pointLightShadowStorageBufferDesc = {
            .elementCapacity = 16,
            .elementStride = sizeof(PointLightShadowConstants),
            .memoryUsage = MemoryUsage::CpuToGpu
        };
        frameResources[i].pointLightShadowStorageBuffer.reset(device.CreateStorageBuffer(pointLightShadowStorageBufferDesc));

        frameResources[i].descriptorSet.reset(device.CreateDescriptorSet(frameDescriptorSetLayout.get()));
        frameResources[i].descriptorSet->WriteUniformBuffer(0, *frameResources[i].uniformBuffer);
        frameResources[i].descriptorSet->WriteStorageBuffer(1, *frameResources[i].pointLightStorageBuffer);
        frameResources[i].descriptorSet->WriteStorageBuffer(2, *frameResources[i].spotLightStorageBuffer);
        frameResources[i].descriptorSet->WriteUniformBuffer(3, *frameResources[i].shadowUniformBuffer);
        frameResources[i].descriptorSet->WriteSampledImage(4, resourceManager.GetShadowSystem().GetDirectionalTarget()->GetDepthView(), VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
        frameResources[i].descriptorSet->WriteSampler(5, *shadowSampler);
        frameResources[i].descriptorSet->WriteSampledImage(6, resourceManager.GetShadowSystem().GetShadowAtlasRT()->GetDepthView(), VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
        frameResources[i].descriptorSet->WriteStorageBuffer(7, *frameResources[i].spotLightShadowStorageBuffer);
        frameResources[i].descriptorSet->WriteSampledImage(8, resourceManager.GetShadowSystem().GetPointShadowSamplingView(), VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
        frameResources[i].descriptorSet->WriteStorageBuffer(9, *frameResources[i].pointLightShadowStorageBuffer);
    }

    return true;
}

void Renderer::DestroyFrameResources()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        frameResources[i].descriptorSet.reset();

        frameResources[i].pointLightShadowStorageBuffer.reset();
        frameResources[i].spotLightShadowStorageBuffer.reset();
        frameResources[i].shadowUniformBuffer.reset();
        frameResources[i].spotLightStorageBuffer.reset();
        frameResources[i].pointLightStorageBuffer.reset();
        frameResources[i].uniformBuffer.reset();

        frameResources[i].commandBuffer.reset();

        if (frameResources[i].gpuTimeStamps.queryPool != VK_NULL_HANDLE)
        {
            vkDestroyQueryPool(
                device.GetVKDevice(),
                frameResources[i].gpuTimeStamps.queryPool,
                nullptr);

            frameResources[i].gpuTimeStamps.queryPool = VK_NULL_HANDLE;
        }
        frameResources[i].gpuTimeStamps.hasPendingResults = false;
        frameResources[i].gpuTimeStamps.submissionFrameNumber = 0;

        if (frameResources[i].inFlightFence != VK_NULL_HANDLE)
        {
            vkDestroyFence(
                device.GetVKDevice(),
                frameResources[i].inFlightFence,
                nullptr);
            frameResources[i].inFlightFence = VK_NULL_HANDLE;
        }

        if (frameResources[i].imageAvailableSemaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(
                device.GetVKDevice(),
                frameResources[i].imageAvailableSemaphore,
                nullptr);
            frameResources[i].imageAvailableSemaphore = VK_NULL_HANDLE;
        }
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

    renderTargetRegistry->RecreateViewportRelative(extent);

    viewportRenderTargetsRecreatedRay.Emit();
}

bool Renderer::ConsumeGPUTimestampResults(FrameResource& frameResource)
{
    std::array<uint64, GpuTimestampQueryCount> timestamps = {};

    const VkResult result = vkGetQueryPoolResults(
        device.GetVKDevice(),
        frameResource.gpuTimeStamps.queryPool,
        0,
        GpuTimestampQueryCount,
        sizeof(timestamps),
        timestamps.data(),
        sizeof(uint64),
        VK_QUERY_RESULT_64_BIT);

    if (result != VK_SUCCESS)
    {
        URAY_LOG(
            "[Renderer] Failed to read GPU timestamp queries. (VkResult: %d)",
            static_cast<int>(result));
        return false;
    }

    if (performanceViewModel)
    {
        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(device.GetPhysicalDevice(), &properties);

        const double tickToMs = static_cast<double>(properties.limits.timestampPeriod) / 1'000'000.0;

        uint64 firstTimestamp = timestamps[0];
        for (uint32 passIndex = 1; passIndex < static_cast<uint32>(RenderPassId::Count); ++passIndex)
        {
            firstTimestamp = std::min(firstTimestamp, timestamps[passIndex * 2]);
        }

        std::vector<ProfileSample> samples;
        samples.reserve(static_cast<size_t>(RenderPassId::Count));

        for (uint32 passIndex = 0; passIndex < static_cast<uint32>(RenderPassId::Count); ++passIndex)
        {
            const uint64 beginTimestamp = timestamps[passIndex * 2];
            const uint64 endTimestamp = timestamps[passIndex * 2 + 1];
            const RenderPassId passId = static_cast<RenderPassId>(passIndex);

            samples.push_back(ProfileSample{
                .name = std::string(GetRenderPassName(passId)),
                .startMs = static_cast<double>(beginTimestamp - firstTimestamp) * tickToMs,
                .durationMs = static_cast<double>(endTimestamp - beginTimestamp) * tickToMs,
                .depth = 0 });
        }

        performanceViewModel->AddGPUFrame(
            frameResource.gpuTimeStamps.submissionFrameNumber,
            samples);
    }

    frameResource.gpuTimeStamps.hasPendingResults = false;
    return true;
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
