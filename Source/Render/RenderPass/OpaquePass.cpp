#include "OpaquePass.h"

#include "Render/GPUResourceManager.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/Renderer.h"

#include <vulkan/vulkan.h>

namespace URay::Render
{

OpaquePass::OpaquePass() = default;

OpaquePass::~OpaquePass() = default;

void OpaquePass::Begin(const RenderPassContext& context)
{
    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = { .float32 = { 0.01f, 0.01f, 0.01f, 1.0f } };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = {
        context.sceneRenderTarget.GetExtent().width,
        context.sceneRenderTarget.GetExtent().height
    };

    context.commandBuffer.BeginRenderPass(
        context.sceneRenderPass,
        context.sceneFramebuffer,
        renderArea,
        clearValues);

    context.commandBuffer.SetViewport(
        0.0f,
        static_cast<float>(context.sceneRenderTarget.GetExtent().height),
        static_cast<float>(context.sceneRenderTarget.GetExtent().width),
        -static_cast<float>(context.sceneRenderTarget.GetExtent().height),
        0.0f,
        1.0f);
    context.commandBuffer.SetScissor(
        0,
        0,
        context.sceneRenderTarget.GetExtent().width,
        context.sceneRenderTarget.GetExtent().height);
}

void OpaquePass::End(const RenderPassContext& context)
{
}

void OpaquePass::Execute(
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

        if (cmd.descriptorSets[1])
        {
            commandBuffer.BindDescriptorSet(
                *pso->GetLayout(),
                *cmd.descriptorSets[1],
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
