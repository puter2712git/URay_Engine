#include "OverlayPass.h"

#include "Render/Renderer.h"

namespace URay::Render
{

OverlayPass::OverlayPass() = default;

OverlayPass::~OverlayPass() = default;

void OverlayPass::Begin(const RenderPassContext& context)
{
    context.renderer.ClearSceneDepth(0.0f, 0);
}

void OverlayPass::End(const RenderPassContext& context)
{
    context.renderer.EndScenePass();
}

void OverlayPass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>& drawCmds)
{
    Renderer& renderer = context.renderer;

    for (const DrawCommand& cmd : drawCmds)
    {
        renderer.Draw(cmd);
    }
}

} // namespace URay::Render
