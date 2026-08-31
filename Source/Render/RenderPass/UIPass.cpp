#include "UIPass.h"

#include "Render/ImGui/ImGuiDrawable.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/Renderer.h"

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
    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = context.swapChainExtent;

    context.commandBuffer.BeginRenderPass(
        context.swapChainRenderPass,
        context.swapChainFramebuffer,
        renderArea,
        clearValues);

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
    context.commandBuffer.EndRenderPass();
}

} // namespace URay::Render
