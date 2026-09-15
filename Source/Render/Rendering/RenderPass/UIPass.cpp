#include "UIPass.h"

#include "Render/RHI/Attachment/RenderingInfo.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/CommandBuffer/ImageBarrier.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/Rendering/ImGui/ImGuiDrawable.h"
#include "Render/Rendering/Renderer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

UIPass::UIPass(ImGuiDrawable& drawable)
    : drawable(drawable)
{
}

UIPass::~UIPass() = default;

void UIPass::Begin(const RenderPassContext& context)
{
    BeginImGui();
}

void UIPass::End(const RenderPassContext& context)
{
    BeginSwapChainPass(context);
    EndImGui(context);
    EndSwapChainPass(context);
}

void UIPass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>& drawCmds)
{
    drawable.DrawImGui();
}

void UIPass::BeginImGui()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIPass::EndImGui(const RenderPassContext& context)
{
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();

    ImGui_ImplVulkan_RenderDrawData(
        drawData,
        context.commandBuffer.GetHandle());
}

void UIPass::BeginSwapChainPass(const RenderPassContext& context)
{
    const VkImageSubresourceRange colorRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const ImageBarrierDesc toColorAttachment = {
        .image = context.swapChainImage,
        .subresourceRange = colorRange,
        .oldLayout = ImageLayout::Undefined,
        .newLayout = ImageLayout::ColorAttachment,
        .srcStage = VK_PIPELINE_STAGE_2_NONE,
        .srcAccess = VK_ACCESS_2_NONE,
        .dstStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
    };
    context.commandBuffer.PipelineBarrier(
        std::span(&toColorAttachment, 1));

    RenderingAttachmentInfo colorAttachment = {};
    colorAttachment.imageView = context.swapChainImageView;
    colorAttachment.layout = ImageLayout::ColorAttachment;
    colorAttachment.loadOp = LoadOp::Clear;
    colorAttachment.storeOp = StoreOp::Store;
    colorAttachment.clearColor = Color::Black;

    const RenderingInfo renderingInfo = {
        .renderArea = { .offset = { 0, 0 }, .extent = context.swapChainExtent },
        .layerCount = 1,
        .colorAttachments = std::span(&colorAttachment, 1)
    };

    context.commandBuffer.BeginRendering(renderingInfo);

    context.commandBuffer.SetViewport(
        0.0f,
        static_cast<float>(context.swapChainExtent.height),
        static_cast<float>(context.swapChainExtent.width),
        -static_cast<float>(context.swapChainExtent.height),
        0.0f,
        1.0f);
    context.commandBuffer.SetScissor(
        0,
        0,
        context.swapChainExtent.width,
        context.swapChainExtent.height);
}

void UIPass::EndSwapChainPass(const RenderPassContext& context)
{
    context.commandBuffer.EndRendering();

    const VkImageSubresourceRange colorRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const ImageBarrierDesc toPresent = {
        .image = context.swapChainImage,
        .subresourceRange = colorRange,
        .oldLayout = ImageLayout::ColorAttachment,
        .newLayout = ImageLayout::Present,
        .srcStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStage = VK_PIPELINE_STAGE_2_NONE,
        .dstAccess = VK_ACCESS_2_NONE,
    };
    context.commandBuffer.PipelineBarrier(
        std::span(&toPresent, 1));
}

} // namespace URay::Render
