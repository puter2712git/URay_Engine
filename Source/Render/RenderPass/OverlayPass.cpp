#include "OverlayPass.h"

#include "Render/GPUResourceManager.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/Renderer.h"

namespace URay::Render
{

OverlayPass::OverlayPass() = default;

OverlayPass::~OverlayPass() = default;

void OverlayPass::Begin(const RenderPassContext& context)
{
    // context.commandBuffer.ClearDepth(
    //     0.0f,
    //     0,
    //     context.sceneRenderTarget.GetExtent().width,
    //     context.sceneRenderTarget.GetExtent().height);
}

void OverlayPass::End(const RenderPassContext& context)
{
    context.commandBuffer.EndRenderPass();
}

void OverlayPass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>& drawCmds)
{
    CommandBuffer& commandBuffer = context.commandBuffer;
    GPUResourceManager& resourceManager = context.resourceManager;

    for (const DrawCommand& cmd : drawCmds)
    {
        PipelineState* pso = resourceManager.GetOrCreatePSO(
            cmd.pipelineState,
            context.sceneRenderPass);

        commandBuffer.BindPipeline(*pso);

        commandBuffer.BindDescriptorSet(
            *pso->GetLayout(),
            context.frameDescriptorSet,
            0);

        if (cmd.descriptorSet)
        {
            commandBuffer.BindDescriptorSet(
                *pso->GetLayout(),
                *cmd.descriptorSet,
                1);
        }

        ObjectConstants objConstants = {};
        objConstants.world = cmd.worldMatrix;
        objConstants.colorTint = cmd.colorTint;
        objConstants.objectId = cmd.objectId;

        if (pso->GetLayout()->SupportsPushConstants())
        {
            vkCmdPushConstants(
                commandBuffer.GetHandle(),
                pso->GetLayout()->GetHandle(),
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(objConstants),
                &objConstants);
        }

        commandBuffer.BindVertexBuffer(*cmd.vertexBuffer);

        if (cmd.indexBuffer)
        {
            commandBuffer.BindIndexBuffer(*cmd.indexBuffer);
            commandBuffer.DrawIndexed(cmd.indexCount, cmd.indexOffset);
        }
        else
        {
            commandBuffer.Draw(cmd.vertexCount);
        }
    }
}

} // namespace URay::Render
