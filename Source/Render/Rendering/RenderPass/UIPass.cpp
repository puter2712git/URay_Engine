#include "UIPass.h"
#include <Core/Math/Math.h>

#include <Render/RHI/Attachment/RenderingInfo.h>
#include <Render/RHI/Barrier/ImageBarrier.h>
#include <Render/RHI/Barrier/PipelineBarrierDesc.h>
#include <Render/RHI/CommandBuffer/CommandBuffer.h>
#include <Render/RHI/PipelineLayout/PipelineLayout.h>
#include <Render/RHI/PipelineState/PipelineState.h>
#include <Render/RHI/RenderTarget.h>
#include <Render/Rendering/RenderConstants.h>
#include <Render/Rendering/Renderer.h>
#include <Render/ResourceManager.h>

#include <Core/Math/Rect.h>

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cmath>
#include <vector>

namespace URay::Render
{

UIPass::UIPass() = default;

UIPass::~UIPass() = default;

void UIPass::Begin(const RenderPassContext& context)
{
    BeginSwapChainPass(context);
}

void UIPass::End(const RenderPassContext& context)
{
    EndSwapChainPass(context);
}

void UIPass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>& drawCmds)
{
    ExecuteCustomUI(context, drawCmds);
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

    context.commandBuffer.PipelineBarrier(PipelineBarrierDesc{
        .imageBarriers = std::span(&toColorAttachment, 1) });

    RenderingAttachmentInfo colorAttachment = {};
    colorAttachment.imageView = context.swapChainImageView;
    colorAttachment.layout = ImageLayout::ColorAttachment;
    colorAttachment.loadOp = LoadOp::Clear;
    colorAttachment.storeOp = StoreOp::Store;
    colorAttachment.clearColor = ClearColorValue{
        .type = ClearValueType::Float,
        .floatValue = Color::Black
    };

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
    context.commandBuffer.PipelineBarrier(PipelineBarrierDesc{
        .imageBarriers = std::span(&toPresent, 1) });
}

void UIPass::ExecuteCustomUI(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds)
{
    CommandBuffer& commandBuffer = context.commandBuffer;
    ResourceManager& resourceManager = context.resourceManager;

    for (const DrawCommand& cmd : drawCmds)
    {
        PipelineStateDesc psoDesc = cmd.pipelineState;
        psoDesc.rendering = {
            .colorAttachmentFormats = { Format::BGRA8_sRGB },
            .depthAttachmentFormat = Format::Unknown,
            .stencilAttachmentFormat = Format::Unknown
        };

        PipelineState* pso = resourceManager.GetOrCreatePSO(psoDesc);

        commandBuffer.BindPipeline(*pso);

        if (cmd.descriptorSets[1] && pso->GetLayout()->HasDescriptorSet(1))
        {
            commandBuffer.BindDescriptorSet(
                *pso->GetLayout(),
                *cmd.descriptorSets[1],
                1);
        }

        UIConstants uiConstants = {
            .viewportSize = {
                static_cast<float>(context.swapChainExtent.width),
                static_cast<float>(context.swapChainExtent.height) }
        };

        if (pso->GetLayout()->SupportsPushConstants())
        {
            commandBuffer.PushConstants(
                *pso->GetLayout(),
                0,
                0,
                sizeof(uiConstants),
                &uiConstants);
        }

        commandBuffer.BindVertexBuffer(*cmd.vertexBuffer);

        if (cmd.scissor.has_value())
        {
            const Rect& rect = *cmd.scissor;

            const int maxX = static_cast<int>(context.swapChainExtent.width);
            const int maxY = static_cast<int>(context.swapChainExtent.height);

            const int left = Math::Clamp(static_cast<int>(std::floor(rect.position.x)), 0, maxX);
            const int top = Math::Clamp(static_cast<int>(std::floor(rect.position.y)), 0, maxY);
            const int right = Math::Clamp(static_cast<int>(std::ceil(rect.position.x + rect.size.x)), 0, maxX);
            const int bottom = Math::Clamp(static_cast<int>(std::ceil(rect.position.y + rect.size.y)), 0, maxY);

            commandBuffer.SetScissor(
                left,
                top,
                static_cast<uint32>(Math::Max(0, right - left)),
                static_cast<uint32>(Math::Max(0, bottom - top)));
        }
        else
        {
        }

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
