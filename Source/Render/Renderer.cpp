#include "Renderer.h"

#include "Engine/Asset/Material/MaterialManager.h"
#include "Render/GPUResourceManager.h"
#include "Render/RHI/Buffer/ConstantBuffer.h"
#include "Render/RHI/Buffer/IndexBuffer.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/CommandBuffer/CommandPool.h"
#include "Render/RHI/CommandBuffer/RenderPassBeginInfo.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Descriptor/DescriptorSetLayout.h"
#include "Render/RHI/Framebuffer.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/SwapChain.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RHI/Vulkan/VulkanContext.h"
#include "Render/RenderInfo.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/Scene/RenderScene.h"
#include "Render/Shader/Shader.h"
#include "Render/Shader/ShaderManager.h"

#include "Engine/Asset/Material/Material.h"
#include "Engine/Scene/Scene.h"

#include "Core/File/VirtualFilesystem.h"

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

Renderer::Renderer(Window& window, VulkanContext& context, RenderDevice& device, GPUResourceManager& resourceManager)
    : window(window), context(context), device(device), resourceManager(resourceManager)
{
}

Renderer::~Renderer() = default;

bool Renderer::Initialize(VirtualFilesystem& filesystem)
{
    glfwSetWindowUserPointer(window.GetGLFWWindow(), this);
    glfwSetFramebufferSizeCallback(window.GetGLFWWindow(), FramebufferResizeCallback);

    int width, height;
    glfwGetFramebufferSize(window.GetGLFWWindow(), &width, &height);

    SwapChainDesc swapChainDesc = {};
    swapChainDesc.extent = {
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
    };

    swapChain.reset(device.CreateSwapChain(swapChainDesc));

    if (!CreateCommandPool())
        return false;

    if (!CreateSceneRenderPass())
        return false;
    if (!CreateRenderPass())
        return false;

    if (!CreateSceneRenderTarget())
        return false;
    if (!CreateSceneFramebuffer())
        return false;

    if (!CreateDepthResources())
        return false;

    swapChainFramebuffers.resize(swapChain->GetImageViews().size());
    for (size_t i = 0; i < swapChain->GetImageViews().size(); ++i)
    {
        std::array<VkImageView, 2> attachments = { swapChain->GetImageView(i), depthImageView };

        FramebufferDesc desc = {};
        desc.renderPass = swapChainRenderPass;
        desc.attachments = attachments;
        desc.extent = swapChain->GetExtent();

        Framebuffer* framebuffer = device.CreateFramebuffer(desc);
        if (!framebuffer)
            return false;

        swapChainFramebuffers[i].reset(framebuffer);
    }

    if (!CreateCommandBuffer())
        return false;
    if (!CreateSyncObjects())
        return false;

    if (!CreateFrameConstantBuffer())
        return false;
    if (!CreateFrameDescriptorSetLayout())
        return false;
    if (!CreateFrameDescriptorSet())
        return false;

    return true;
}

void Renderer::Finalize()
{
    vkDeviceWaitIdle(device.GetVKDevice());

    CleanupSwapChain();

    DestroyFrameDescriptorSet();
    DestroyFrameDescriptorSetLayout();
    DestroyFrameConstantBuffer();

    DestroyDepthResources();

    DestroySceneFramebuffer();
    DestroySceneRenderTarget();

    DestroyRenderPass();
    DestroySceneRenderPass();

    DestroySyncObjects();

    DestroyCommandPool();
}

bool Renderer::InitializeImGui(const VirtualFilesystem& filesystem)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    const std::vector<uint8_t> fontBytes = filesystem.ReadBinary("RawAsset://Font/PretendardVariable.ttf");
    if (fontBytes.empty())
        return false;

    void* fontData = IM_ALLOC(fontBytes.size());
    std::memcpy(fontData, fontBytes.data(), fontBytes.size());

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = true;

    if (!io.Fonts->AddFontFromMemoryTTF(
            fontData,
            static_cast<int>(fontBytes.size()),
            18.0f,
            &fontConfig,
            io.Fonts->GetGlyphRangesKorean()))
    {
        IM_FREE(fontData);
        return false;
    }

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window.GetGLFWWindow(), true);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.ApiVersion = VK_API_VERSION_1_1;
    initInfo.Instance = context.GetInstance();
    initInfo.PhysicalDevice = device.GetPhysicalDevice();
    initInfo.Device = device.GetVKDevice();
    initInfo.QueueFamily = device.FindQueueFamilyIndices(device.GetPhysicalDevice()).graphicsFamily.value();
    initInfo.Queue = device.GetGraphicsQueue();
    initInfo.DescriptorPool = VK_NULL_HANDLE;
    initInfo.DescriptorPoolSize = 8;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = static_cast<uint32_t>(swapChain->GetImageViews().size());
    initInfo.PipelineInfoMain.RenderPass = swapChainRenderPass;
    initInfo.PipelineInfoMain.Subpass = 0;
    ImGui_ImplVulkan_Init(&initInfo);

    sceneImGuiTexture = ImGui_ImplVulkan_AddTexture(
        sceneRenderTarget->GetColorView()->GetHandle(),
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
    vkWaitForFences(device.GetVKDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    ProcessPendingSceneRenderTargetResize();

    VkResult result = swapChain->AcquireNextImage(imageAvailableSemaphores[currentFrame], imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || result == VK_NOT_READY)
    {
        RecreateSwapChain();
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image.");
    }

    vkResetFences(device.GetVKDevice(), 1, &inFlightFences[currentFrame]);

    if (!commandBuffers[currentFrame]->Reset())
        return false;

    if (!commandBuffers[currentFrame]->Begin(CommandBufferUsage::None))
        return false;

    FrameConstants frameConstants = {};
    frameConstants.view = viewMatrix;
    frameConstants.proj = projMatrix;

    frameConstantBuffers[currentFrame]->UpdateData(
        &frameConstants,
        sizeof(FrameConstants));

    return true;
}

void Renderer::EndFrame()
{
    if (!commandBuffers[currentFrame]->End())
        return;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkCommandBuffer vkCommandBuffer = commandBuffers[currentFrame]->GetHandle();

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
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

void Renderer::BeginScenePass()
{
    RenderPassBeginInfo beginInfo = {};
    beginInfo.renderPass = sceneRenderPass;
    beginInfo.framebuffer = sceneFramebuffer->GetHandle();
    beginInfo.renderArea.offset = { 0, 0 };
    beginInfo.renderArea.extent = {
        .width = sceneRenderTarget->GetExtent().width,
        .height = sceneRenderTarget->GetExtent().height,
    };

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = { .float32 = { 0.01f, 0.01f, 0.01f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    beginInfo.clearValues = clearValues;

    commandBuffers[currentFrame]->BeginRenderPass(beginInfo);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(sceneRenderTarget->GetExtent().height);
    viewport.width = static_cast<float>(sceneRenderTarget->GetExtent().width);
    viewport.height = -static_cast<float>(sceneRenderTarget->GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffers[currentFrame]->GetHandle(), 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = {
        .width = sceneRenderTarget->GetExtent().width,
        .height = sceneRenderTarget->GetExtent().height,
    };
    vkCmdSetScissor(commandBuffers[currentFrame]->GetHandle(), 0, 1, &scissor);
}

void Renderer::EndScenePass()
{
    commandBuffers[currentFrame]->EndRenderPass();
}

void Renderer::BeginSwapChainPass()
{
    const VkExtent2D swapChainExtent = swapChain->GetExtent();

    RenderPassBeginInfo beginInfo = {};
    beginInfo.renderPass = swapChainRenderPass;
    beginInfo.framebuffer = swapChainFramebuffers[imageIndex]->GetHandle();
    beginInfo.renderArea.offset = { 0, 0 };
    beginInfo.renderArea.extent = swapChainExtent;

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    beginInfo.clearValues = clearValues;

    commandBuffers[currentFrame]->BeginRenderPass(beginInfo);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(swapChainExtent.height);
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = -static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffers[currentFrame]->GetHandle(), 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffers[currentFrame]->GetHandle(), 0, 1, &scissor);
}

void Renderer::EndSwapChainPass()
{
    commandBuffers[currentFrame]->EndRenderPass();
}

void Renderer::BeginImGui()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::EndImGui()
{
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();

    ImGui_ImplVulkan_RenderDrawData(
        drawData,
        commandBuffers[currentFrame]->GetHandle());
}

void Renderer::WaitIdle()
{
    vkDeviceWaitIdle(device.GetVKDevice());
}

void Renderer::SetFrameViewInfo(const Matrix& newViewMatrix, const Matrix& newProjMatrix)
{
    viewMatrix = newViewMatrix;
    projMatrix = newProjMatrix;
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

void Renderer::ClearSceneDepth(float depth, uint32_t stencil)
{
    VkClearAttachment attachment = {};
    attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    attachment.clearValue.depthStencil = {
        .depth = depth,
        .stencil = stencil,
    };

    VkClearRect rect = {};
    rect.rect.offset = { 0, 0 };
    rect.rect.extent = {
        sceneRenderTarget->GetExtent().width,
        sceneRenderTarget->GetExtent().height,
    };
    rect.baseArrayLayer = 0;
    rect.layerCount = 1;

    vkCmdClearAttachments(
        commandBuffers[currentFrame]->GetHandle(),
        1,
        &attachment,
        1,
        &rect);
}

void Renderer::Draw(const DrawCommand& cmd)
{
    PipelineState* pso = resourceManager.GetOrCreatePSO(cmd.pipelineState, sceneRenderPass);
    vkCmdBindPipeline(commandBuffers[currentFrame]->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pso->GetHandle());

    VkDescriptorSet vkFrameDescriptorSet = frameDescriptorSets[currentFrame]->GetHandle();
    vkCmdBindDescriptorSets(commandBuffers[currentFrame]->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pso->GetLayout()->GetHandle(), 0, 1, &vkFrameDescriptorSet, 0, nullptr);

    if (cmd.descriptorSet)
    {
        VkDescriptorSet vkDescriptorSet = cmd.descriptorSet->GetHandle();
        if (vkDescriptorSet != VK_NULL_HANDLE)
        {
            vkCmdBindDescriptorSets(commandBuffers[currentFrame]->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pso->GetLayout()->GetHandle(), 1, 1, &vkDescriptorSet, 0, nullptr);
        }
    }

    ObjectConstants objConstants = {};
    objConstants.world = cmd.worldMatrix;
    objConstants.colorTint = cmd.colorTint;
    objConstants.objectId = cmd.objectId;

    if (pso->GetLayout()->SupportsPushConstants())
    {
        vkCmdPushConstants(commandBuffers[currentFrame]->GetHandle(), pso->GetLayout()->GetHandle(),
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0, sizeof(objConstants), &objConstants);
    }

    VkBuffer vertexBuffers[] = { static_cast<VkBuffer>(cmd.vertexBuffer) };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffers[currentFrame]->GetHandle(), 0, 1, vertexBuffers, offsets);

    if (cmd.indexBuffer)
    {
        VkBuffer indexBuffer = static_cast<VkBuffer>(cmd.indexBuffer);
        vkCmdBindIndexBuffer(commandBuffers[currentFrame]->GetHandle(), indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(commandBuffers[currentFrame]->GetHandle(), cmd.indexCount, 1, cmd.indexOffset, 0, 0);
    }
    else
    {
        vkCmdDraw(commandBuffers[currentFrame]->GetHandle(), cmd.vertexCount, 1, 0, 0);
    }
}

void Renderer::CleanupSwapChain()
{
    for (auto& framebuffer : swapChainFramebuffers)
        framebuffer.reset();

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
    DestroyDepthResources();

    SwapChainDesc swapChainDesc = {};
    swapChainDesc.extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
    };
    swapChain.reset(device.CreateSwapChain(swapChainDesc));
    if (!swapChain)
        return;

    swapChainFramebuffers.resize(swapChain->GetImageViews().size());

    CreateDepthResources();

    for (size_t i = 0; i < swapChain->GetImageViews().size(); ++i)
    {
        std::array<VkImageView, 2> attachments = { swapChain->GetImageView(i), depthImageView };

        FramebufferDesc desc = {};
        desc.renderPass = swapChainRenderPass;
        desc.attachments = attachments;
        desc.extent = swapChain->GetExtent();

        Framebuffer* framebuffer = device.CreateFramebuffer(desc);
        if (!framebuffer)
            return;

        swapChainFramebuffers[i].reset(framebuffer);
    }
}

bool Renderer::CreateSceneRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency sceneToShaderRead = {};
    sceneToShaderRead.srcSubpass = 0;
    sceneToShaderRead.dstSubpass = VK_SUBPASS_EXTERNAL;
    sceneToShaderRead.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    sceneToShaderRead.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    sceneToShaderRead.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    sceneToShaderRead.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    std::array<VkSubpassDependency, 2> dependencies = { dependency, sceneToShaderRead };

    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    if (vkCreateRenderPass(device.GetVKDevice(), &renderPassInfo, nullptr, &sceneRenderPass) != VK_SUCCESS)
        return false;

    return true;
}

void Renderer::DestroySceneRenderPass()
{
    if (sceneRenderPass)
    {
        vkDestroyRenderPass(device.GetVKDevice(), sceneRenderPass, nullptr);
    }
}

bool Renderer::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChain->GetFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = FindDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device.GetVKDevice(), &renderPassInfo, nullptr, &swapChainRenderPass) != VK_SUCCESS)
        return false;

    return true;
}

void Renderer::DestroyRenderPass()
{
    if (swapChainRenderPass)
    {
        vkDestroyRenderPass(device.GetVKDevice(), swapChainRenderPass, nullptr);
    }
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
        sceneRenderTarget = nullptr;
    }
}

bool Renderer::CreateSceneFramebuffer()
{
    std::array<VkImageView, 2> attachments = { sceneRenderTarget->GetColorView()->GetHandle(),
                                               sceneRenderTarget->GetDepthView()->GetHandle() };

    FramebufferDesc desc = {};
    desc.renderPass = sceneRenderPass;
    desc.attachments = attachments;
    desc.extent = {
        .width = sceneRenderTarget->GetExtent().width,
        .height = sceneRenderTarget->GetExtent().height,
    };

    sceneFramebuffer.reset(device.CreateFramebuffer(desc));
    if (!sceneFramebuffer)
        return false;

    return true;
}

void Renderer::DestroySceneFramebuffer()
{
    if (sceneFramebuffer)
    {
        sceneFramebuffer.reset();
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
        commandBuffers.clear();
        commandPool.reset();
    }
}

bool Renderer::CreateCommandBuffer()
{
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        commandBuffers[i].reset(commandPool->Allocate());
    }

    return true;
}

bool Renderer::CreateSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (vkCreateSemaphore(device.GetVKDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device.GetVKDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device.GetVKDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {
            return false;
        }
    }

    return true;
}

void Renderer::DestroySyncObjects()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(device.GetVKDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device.GetVKDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(device.GetVKDevice(), inFlightFences[i], nullptr);
    }
}

bool Renderer::CreateDepthResources()
{
    VkFormat depthFormat = FindDepthFormat();
    VkExtent2D swapChainExtent = swapChain->GetExtent();

    device.CreateImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       depthImage, depthImageMemory);
    depthImageView = device.CreateImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    return true;
}

void Renderer::DestroyDepthResources()
{
    vkDestroyImageView(device.GetVKDevice(), depthImageView, nullptr);
    vkDestroyImage(device.GetVKDevice(), depthImage, nullptr);
    vkFreeMemory(device.GetVKDevice(), depthImageMemory, nullptr);
}

bool Renderer::CreateFrameDescriptorSetLayout()
{
    ResourceBinding binding = {};
    binding.set = 0;
    binding.bindingIndex = 0;
    binding.arrayCount = 1;
    binding.resourceType = ResourceType::ConstantBuffer;
    binding.stageFlags = ShaderStageFlags::Vertex;

    DescriptorSetLayoutDesc desc = {};
    desc.bindings.push_back(binding);

    frameDescriptorSetLayout = device.CreateDescriptorSetLayout(desc);

    if (!frameDescriptorSetLayout)
        return false;

    return true;
}

void Renderer::DestroyFrameDescriptorSetLayout()
{
    if (frameDescriptorSetLayout)
    {
        delete frameDescriptorSetLayout;
        frameDescriptorSetLayout = nullptr;
    }
}

bool Renderer::CreateFrameDescriptorSet()
{
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        DescriptorSet* descriptorSet = device.CreateDescriptorSet(frameDescriptorSetLayout);

        if (!descriptorSet)
            return false;

        frameDescriptorSets.push_back(descriptorSet);

        descriptorSet->WriteUniformBuffer(0, frameConstantBuffers[i].get());
    }

    return true;
}

void Renderer::DestroyFrameDescriptorSet()
{
    for (auto& descriptorSet : frameDescriptorSets)
    {
        if (descriptorSet)
        {
            delete descriptorSet;
            descriptorSet = nullptr;
        }
    }

    frameDescriptorSets.clear();
}

bool Renderer::CreateFrameConstantBuffer()
{
    frameConstantBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkDeviceSize frameConstantSize = sizeof(FrameConstants);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkBuffer handle = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        device.CreateBuffer(
            frameConstantSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            handle,
            memory);

        frameConstantBuffers[i] = std::make_unique<ConstantBuffer>(
            device.GetVKDevice(), handle, memory, frameConstantSize);
    }

    return true;
}

void Renderer::DestroyFrameConstantBuffer()
{
    for (auto& frameConstantBuffer : frameConstantBuffers)
    {
        frameConstantBuffer.reset();
    }
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

    DestroySceneFramebuffer();

    if (!sceneRenderTarget->Resize(extent))
    {
        throw std::runtime_error("Failed to resize scene render target.");
    }

    if (!CreateSceneFramebuffer())
    {
        throw std::runtime_error("Failed to recreate scene framebuffer.");
    }

    sceneImGuiTexture = ImGui_ImplVulkan_AddTexture(
        sceneRenderTarget->GetColorView()->GetHandle(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VkFormat Renderer::FindDepthFormat() const
{
    return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                               VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
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
