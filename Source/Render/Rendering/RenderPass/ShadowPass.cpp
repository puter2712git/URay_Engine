#include "ShadowPass.h"

#include "Render/RHI/Attachment/RenderingInfo.h"
#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Buffer/BufferDesc.h"
#include "Render/RHI/CommandBuffer/CommandBuffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/PipelineLayout/PipelineLayout.h"
#include "Render/RHI/PipelineState/PipelineState.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RenderSystem.h"
#include "Render/Rendering/FrameResource.h"
#include "Render/Rendering/Object/Light/DirectionalLightObject.h"
#include "Render/Rendering/RenderConstants.h"
#include "Render/Rendering/RenderInfo.h"
#include "Render/Rendering/Shadow/ShadowSystem.h"
#include "Render/Rendering/View/RenderView.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Engine.h"

namespace URay::Render
{

ShadowPass::ShadowPass()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    RenderSystem& renderSystem = gEngine->GetRenderSystem();

    RenderDevice& device = renderSystem.GetDevice();
    ResourceManager& resourceManager = renderSystem.GetResourceManager();

    std::vector<URay::Shader*> shaders = assetSystem.FindAssets<URay::Shader>();
    for (URay::Shader* shader : shaders)
    {
        if (shader->GetName() == "Shadow")
        {
            shadowShaderAsset = shader;
            break;
        }
    }

    shadowShader = resourceManager.GetOrCreateShader(shadowShaderAsset, {});
}

ShadowPass::~ShadowPass() = default;

void ShadowPass::Begin(const RenderPassContext& context)
{
    ShadowSystem& shadowSystem = context.resourceManager.GetShadowSystem();

    RenderTarget* renderTarget = shadowSystem.GetDirectionalTarget();
    renderTarget->TransitionDepth(context.commandBuffer, ImageLayout::DepthAttachment);

    const Extent2D& extent = renderTarget->GetExtent();

    const RenderingAttachmentInfo depthAttachment = {
        .imageView = renderTarget->GetDepthView()->GetHandle(),
        .layout = ImageLayout::DepthAttachment,
        .loadOp = LoadOp::Clear,
        .storeOp = StoreOp::Store,
        .clearDepth = 1.0f,
        .clearStencil = 0
    };

    const RenderingInfo renderingInfo = {
        .renderArea = {
            .offset = { 0, 0 },
            .extent = { extent.width, extent.height } },
        .layerCount = 1,
        .colorAttachments = {},
        .depthAttachment = &depthAttachment
    };

    context.commandBuffer.BeginRendering(renderingInfo);
    context.commandBuffer.SetViewport(
        0.0f,
        static_cast<float>(extent.height),
        static_cast<float>(extent.width),
        -static_cast<float>(extent.height),
        0.0f,
        1.0f);
    context.commandBuffer.SetScissor(
        0, 0, extent.width, extent.height);
}

void ShadowPass::End(const RenderPassContext& context)
{
    ShadowSystem& shadowSystem = context.resourceManager.GetShadowSystem();

    RenderTarget* renderTarget = shadowSystem.GetDirectionalTarget();

    context.commandBuffer.EndRendering();

    renderTarget->TransitionDepth(context.commandBuffer, ImageLayout::DepthReadOnly);
}

void ShadowPass::Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds)
{
    DirectionalLightObject* directionalLight = context.directionalLight;
    if (!directionalLight)
        return;

    RenderView& renderView = context.renderView;

    const Vector3& cameraPosition = renderView.cameraPosition;
    const Vector3& cameraDirection = renderView.cameraDirection;

    const Vector3& lightDirection = directionalLight->GetDirection();

    const Vector3 focus = Vector3::Zero; // TODO: Set light position by adequate way

    const Vector3 lightEye = focus - lightDirection * 100.0f;
    const Vector3 target = lightEye + lightDirection;
    const Vector3 lightUp = std::abs(Vector3::Dot(lightDirection, Vector3::Up)) > 0.99f
                                ? Vector3::Right
                                : Vector3::Up;

    const Matrix lightView = Matrix::MakeView(lightEye, target, lightUp);
    const Matrix lightProj = Matrix::MakeOrthogonal(-50.0f, 50.0f, -50.0f, 50.0f, renderView.nearPlane, renderView.farPlane);

    ShadowConstants constants = {};
    constants.lightViewProj = lightView * lightProj;
    constants.bias = directionalLight->GetBias();

    context.shadowUniformBuffer.Update(&constants, sizeof(constants));

    CommandBuffer& commandBuffer = context.commandBuffer;
    ResourceManager& resourceManager = context.resourceManager;

    for (const DrawCommand& cmd : drawCmds)
    {
        PipelineStateDesc psoDesc = cmd.pipelineState;
        psoDesc.shader = shadowShader;
        psoDesc.rendering = PipelineRenderingDesc{
            .colorAttachmentFormats = {},
            .depthAttachmentFormat = Format::D32_Float,
            .stencilAttachmentFormat = Format::Unknown
        };

        PipelineState* pso = resourceManager.GetOrCreatePSO(psoDesc);

        commandBuffer.BindPipeline(*pso);

        commandBuffer.BindDescriptorSet(
            *pso->GetLayout(),
            context.frameDescriptorSet,
            0);

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
