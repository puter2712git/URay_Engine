#include "OverlayPass.h"

#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/CommandBuffer/ImageBarrier.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/Rendering/RenderConstants.h"
#include "Render/ResourceManager.h"

#include <vulkan/vulkan.h>

#include <array>

namespace URay::Render
{

OverlayPass::OverlayPass() = default;

OverlayPass::~OverlayPass() = default;

void OverlayPass::Begin(const RenderPassContext& context)
{
}

void OverlayPass::End(const RenderPassContext& context)
{
    context.commandBuffer.EndRendering();

    const VkImageSubresourceRange colorRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    const VkImageSubresourceRange depthRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    const std::array<ImageBarrierDesc, 2> barriers = {
        ImageBarrierDesc{
            .image = context.sceneRenderTarget.GetColorTexture()->GetHandle(),
            .subresourceRange = colorRange,
            .oldLayout = ImageLayout::ColorAttachment,
            .newLayout = ImageLayout::ShaderReadOnly,
            .srcStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            .dstAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT },
        ImageBarrierDesc{
            .image = context.sceneRenderTarget.GetDepthTexture()->GetHandle(),
            .subresourceRange = depthRange,
            .oldLayout = ImageLayout::DepthAttachment,
            .newLayout = ImageLayout::DepthReadOnly,
            .srcStage =
                VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            .srcAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            .dstAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT }
    };

    context.commandBuffer.PipelineBarrier(barriers);
}

void OverlayPass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>& drawCmds)
{
    CommandBuffer& commandBuffer = context.commandBuffer;
    ResourceManager& resourceManager = context.resourceManager;

    for (const DrawCommand& cmd : drawCmds)
    {
        PipelineStateDesc psoDesc = cmd.pipelineState;
        psoDesc.rendering = {
            .colorAttachmentFormats = { Format::BGRA8_sRGB },
            .depthAttachmentFormat = Format::D32_Float_S8_UInt,
            .stencilAttachmentFormat = Format::Unknown
        };

        PipelineState* pso = resourceManager.GetOrCreatePSO(psoDesc);

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
