#include "OpaquePass.h"

#include "Render/RHI/Attachment/RenderingInfo.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/Rendering/FrameResource.h"
#include "Render/Rendering/RenderConstants.h"
#include "Render/ResourceManager.h"

#include <vulkan/vulkan.h>

#include <array>

namespace URay::Render
{

OpaquePass::OpaquePass() = default;

OpaquePass::~OpaquePass() = default;

void OpaquePass::Begin(const RenderPassContext& context)
{
    const Extent2D& extent = context.sceneRenderTarget.GetExtent();

    std::vector<RenderingAttachmentInfo> colorAttachments;
    colorAttachments.push_back(RenderingAttachmentInfo{
        .imageView = context.sceneRenderTarget.GetColorView(0)->GetHandle(),
        .layout = ImageLayout::ColorAttachment,
        .loadOp = LoadOp::Clear,
        .storeOp = StoreOp::Store,
        .clearColor = ClearColorValue{
            .type = ClearValueType::Float,
            .floatValue = Color(0.01f, 0.01f, 0.01f, 1.0f) } });
    colorAttachments.push_back(RenderingAttachmentInfo{
        .imageView = context.sceneRenderTarget.GetColorView(1)->GetHandle(),
        .layout = ImageLayout::ColorAttachment,
        .loadOp = LoadOp::Clear,
        .storeOp = StoreOp::Store,
        .clearColor = ClearColorValue{
            .type = ClearValueType::UInt,
            .uintValue = 0 } });

    const RenderingAttachmentInfo depthAttachment = { .imageView = context.sceneRenderTarget.GetDepthView()->GetHandle(), .layout = ImageLayout::DepthAttachment, .loadOp = LoadOp::Clear, .storeOp = StoreOp::Store, .clearDepth = 1.0f, .clearStencil = 0 };

    const RenderingInfo renderingInfo = {
        .renderArea = {
            .offset = { 0, 0 },
            .extent = { extent.width, extent.height } },
        .layerCount = 1,
        .colorAttachments = colorAttachments,
        .depthAttachment = &depthAttachment
    };

    context.sceneRenderTarget.TransitionColor(context.commandBuffer, 0, ImageLayout::ColorAttachment);
    context.sceneRenderTarget.TransitionColor(context.commandBuffer, 1, ImageLayout::ColorAttachment);
    context.sceneRenderTarget.TransitionDepth(context.commandBuffer, ImageLayout::DepthAttachment);

    context.commandBuffer.BeginRendering(renderingInfo);

    context.commandBuffer.SetViewport(
        0.0f,
        static_cast<float>(extent.height),
        static_cast<float>(extent.width),
        -static_cast<float>(extent.height),
        0.0f,
        1.0f);

    context.commandBuffer.SetScissor(
        0,
        0,
        extent.width,
        extent.height);
}

void OpaquePass::End(const RenderPassContext& context)
{
}

void OpaquePass::Execute(
    const RenderPassContext& context,
    const std::vector<DrawCommand>& drawCmds)
{
    CommandBuffer& commandBuffer = context.commandBuffer;
    ResourceManager& resourceManager = context.resourceManager;

    for (const DrawCommand& cmd : drawCmds)
    {
        PipelineStateDesc psoDesc = cmd.pipelineState;
        psoDesc.rendering = {
            .colorAttachmentFormats = { Format::BGRA8_sRGB, Format::R32_UInt },
            .depthAttachmentFormat = Format::D32_Float_S8_UInt,
            .stencilAttachmentFormat = Format::Unknown
        };
        psoDesc.colorBlendAttachments.push_back(ColorBlendAttachmentState{
            .mode = BlendMode::Opaque,
            .writeMask = ColorWriteMask::None });

        PipelineState* pso = resourceManager.GetOrCreatePSO(psoDesc);

        commandBuffer.BindPipeline(*pso);

        commandBuffer.BindDescriptorSet(
            *pso->GetLayout(),
            *context.frameResource.descriptorSet,
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
