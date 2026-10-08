#include "GizmoPass.h"

#include <Render/RHI/Attachment/RenderingInfo.h>
#include <Render/RHI/CommandBuffer/CommandBuffer.h>
#include <Render/RHI/PipelineLayout/PipelineLayout.h>
#include <Render/RHI/PipelineState/PipelineState.h>
#include <Render/RHI/RenderTarget.h>
#include <Render/RHI/Texture/TextureView.h>
#include <Render/Rendering/FrameResource.h>
#include <Render/Rendering/RenderConstants.h>
#include <Render/Rendering/RenderTargetRegistry.h>
#include <Render/ResourceManager.h>

#include <vulkan/vulkan.h>

#include <array>

namespace URay::Render
{

GizmoPass::GizmoPass() = default;

GizmoPass::~GizmoPass() = default;

void GizmoPass::Begin(const RenderPassContext& context)
{
    RenderTarget& postProcessRT = context.renderTargetRegistry.Get(RenderTargetId::PostProcess);
    RenderTarget& sceneRT = context.renderTargetRegistry.Get(RenderTargetId::Scene);
    RenderTarget& gizmoDepthRT = context.renderTargetRegistry.Get(RenderTargetId::GizmoDepth);

    const Extent2D& extent = postProcessRT.GetExtent();

    const std::array colorAttachments = {
        RenderingAttachmentInfo{
            .imageView = postProcessRT.GetColorView(0)->GetHandle(),
            .layout = ImageLayout::ColorAttachment,
            .loadOp = LoadOp::Load,
            .storeOp = StoreOp::Store },
        RenderingAttachmentInfo{
            .imageView = sceneRT.GetColorView(1)->GetHandle(),
            .layout = ImageLayout::ColorAttachment,
            .loadOp = LoadOp::Load,
            .storeOp = StoreOp::Store }
    };

    const RenderingAttachmentInfo depthAttachment = {
        .imageView = gizmoDepthRT.GetDepthView()->GetHandle(),
        .layout = ImageLayout::DepthAttachment,
        .loadOp = LoadOp::Clear,
        .storeOp = StoreOp::DontCare,
        .clearDepth = 1.0f,
        .clearStencil = 0
    };

    const RenderingInfo renderingInfo = {
        .renderArea = {
            .offset = { 0, 0 },
            .extent = { extent.width, extent.height } },
        .layerCount = 1,
        .colorAttachments = colorAttachments,
        .depthAttachment = &depthAttachment
    };

    postProcessRT.TransitionColor(context.commandBuffer, 0, ImageLayout::ColorAttachment);
    sceneRT.TransitionColor(context.commandBuffer, 1, ImageLayout::ColorAttachment);
    gizmoDepthRT.TransitionDepth(context.commandBuffer, ImageLayout::DepthAttachment);

    context.commandBuffer.BeginRendering(renderingInfo);

    context.commandBuffer.SetViewport(
        0.0f,
        static_cast<float>(extent.height),
        static_cast<float>(extent.width),
        -static_cast<float>(extent.height),
        0.0f,
        1.0f);
    context.commandBuffer.SetScissor(0, 0, extent.width, extent.height);
}

void GizmoPass::End(const RenderPassContext& context)
{
    RenderTarget& postProcessRT = context.renderTargetRegistry.Get(RenderTargetId::PostProcess);

    context.commandBuffer.EndRendering();
    postProcessRT.TransitionColor(context.commandBuffer, 0, ImageLayout::ShaderReadOnly);
}

void GizmoPass::Execute(
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
