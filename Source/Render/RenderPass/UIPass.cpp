#include "UIPass.h"

#include "Render/ImGui/ImGuiDrawable.h"
#include "Render/Renderer.h"

namespace URay::Render
{

UIPass::UIPass(ImGuiDrawable& drawable)
    : drawable(drawable)
{
}

UIPass::~UIPass() = default;

void UIPass::Begin(const RenderPassContext& context)
{
    Renderer& renderer = context.renderer;

    renderer.BeginImGui();
}

void UIPass::End(const RenderPassContext& context)
{
    Renderer& renderer = context.renderer;

    renderer.BeginSwapChainPass();
    renderer.EndImGui();
    renderer.EndSwapChainPass();
}

void UIPass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>& drawCmds)
{
    drawable.DrawImGui();
}

} // namespace URay::Render
