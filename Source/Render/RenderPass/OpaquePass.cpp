#include "OpaquePass.h"

#include "Render/GPUResourceManager.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/Renderer.h"

namespace URay::Render
{

OpaquePass::OpaquePass() = default;

OpaquePass::~OpaquePass() = default;

void OpaquePass::Begin(const RenderPassContext& context)
{
}

void OpaquePass::End(const RenderPassContext& context)
{
}

void OpaquePass::Execute(
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
