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
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RenderSystem.h"
#include "Render/Rendering/FrameResource.h"
#include "Render/Rendering/Object/Light/DirectionalLightObject.h"
#include "Render/Rendering/Object/Light/PointLightObject.h"
#include "Render/Rendering/Object/Light/SpotLightObject.h"
#include "Render/Rendering/RenderConstants.h"
#include "Render/Rendering/RenderInfo.h"
#include "Render/Rendering/Shadow/ShadowSystem.h"
#include "Render/Rendering/View/RenderView.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Engine.h"

#include "Core/Math/Math.h"

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
}

void ShadowPass::End(const RenderPassContext& context)
{
}

void ShadowPass::Execute(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds)
{
    RecordDirectionalDepth(context, drawCmds);
    RecordSpotLightsDepth(context, drawCmds);
    RecordPointLightsDepth(context, drawCmds);
}

void ShadowPass::RecordDirectionalDepth(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds)
{
    DirectionalLightObject* directionalLight = context.directionalLight;
    if (!directionalLight)
        return;

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

    CommandBuffer& commandBuffer = context.commandBuffer;
    ResourceManager& resourceManager = context.resourceManager;

    ShadowConstants directionalLightShadow = {};
    directionalLightShadow.lightViewProj = lightView * lightProj;
    directionalLightShadow.bias = directionalLight->GetBias();

    context.frameResource.shadowUniformBuffer->Update(
        &directionalLightShadow, sizeof(directionalLightShadow));

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

        ShadowMapConstants shadowMapConstants = {};
        shadowMapConstants.world = cmd.worldMatrix;
        shadowMapConstants.lightViewProj = lightView * lightProj;

        if (pso->GetLayout()->SupportsPushConstants())
        {
            vkCmdPushConstants(
                commandBuffer.GetHandle(),
                pso->GetLayout()->GetHandle(),
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(shadowMapConstants),
                &shadowMapConstants);
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

    context.commandBuffer.EndRendering();
    renderTarget->TransitionDepth(context.commandBuffer, ImageLayout::DepthReadOnly);
}

void ShadowPass::RecordSpotLightsDepth(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds)
{
    ShadowSystem& shadowSystem = context.resourceManager.GetShadowSystem();
    RenderTarget* renderTarget = shadowSystem.GetShadowAtlasRT();

    bool firstTime = true;

    std::vector<SpotLightShadowConstants> shadowConstants;

    for (SpotLightObject* spotLight : context.spotLights)
    {
        std::optional<ShadowAtlasEntry> entry = shadowSystem.GetOrAllocateEntry(spotLight);
        if (!entry.has_value())
            continue;

        if (firstTime)
        {
            renderTarget->TransitionDepth(context.commandBuffer, ImageLayout::DepthAttachment);

            const Extent2D& extent = renderTarget->GetExtent();

            const RenderingAttachmentInfo attachment = {
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
                .depthAttachment = &attachment
            };

            context.commandBuffer.BeginRendering(renderingInfo);

            firstTime = false;
        }

        context.commandBuffer.SetViewport(
            static_cast<float>(entry->x),
            static_cast<float>(entry->y + entry->height),
            static_cast<float>(entry->width),
            -static_cast<float>(entry->height),
            0.0f,
            1.0f);

        context.commandBuffer.SetScissor(
            entry->x, entry->y, entry->width, entry->height);

        const Vector3 lightDirection = spotLight->GetDirection();

        const Vector3 eye = spotLight->GetPosition();
        const Vector3 target = eye + lightDirection;
        const Vector3 up = std::abs(Vector3::Dot(lightDirection, Vector3::Up)) > 0.99f
                               ? Vector3::Right
                               : Vector3::Up;

        const Matrix lightView = Matrix::MakeView(eye, target, up);

        const float fov = Math::DegToRad(spotLight->GetOuterConeAngle());
        const float aspect = static_cast<float>(entry->width) / static_cast<float>(entry->height);

        const Matrix lightProj = Matrix::MakePerspective(
            fov, aspect, 0.1f, spotLight->GetRange());

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

            ShadowMapConstants shadowMapConstants = {};
            shadowMapConstants.world = cmd.worldMatrix;
            shadowMapConstants.lightViewProj = lightView * lightProj;

            if (pso->GetLayout()->SupportsPushConstants())
            {
                vkCmdPushConstants(
                    commandBuffer.GetHandle(),
                    pso->GetLayout()->GetHandle(),
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(shadowMapConstants),
                    &shadowMapConstants);
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

        shadowConstants.push_back(SpotLightShadowConstants{
            .lightViewProj = lightView * lightProj,
            .altasUVScaleBias = Vector4(
                static_cast<float>(entry->width) / renderTarget->GetExtent().width,
                static_cast<float>(entry->height) / renderTarget->GetExtent().height,
                static_cast<float>(entry->x) / renderTarget->GetExtent().width,
                static_cast<float>(entry->y) / renderTarget->GetExtent().height),
            .bias = spotLight->GetBias() });

        context.frameResource.spotLightShadowStorageBuffer->Update(
            shadowConstants.data(),
            sizeof(SpotLightShadowConstants) * shadowConstants.size());
    }

    if (!firstTime)
    {
        context.commandBuffer.EndRendering();
        renderTarget->TransitionDepth(context.commandBuffer, ImageLayout::DepthReadOnly);
    }
}

void ShadowPass::RecordPointLightsDepth(const RenderPassContext& context, const std::vector<DrawCommand>& drawCmds)
{
    ShadowSystem& shadowSystem = context.resourceManager.GetShadowSystem();
    Texture* textureCube = shadowSystem.GetDepthTextureCubeArray();

    const TextureDesc& textureDesc = textureCube->GetDesc();
    const uint32 width = textureDesc.width;
    const uint32 height = textureDesc.height;

    std::vector<PointLightShadowConstants> shadowConstants;

    textureCube->Transition(context.commandBuffer, ImageLayout::DepthAttachment);

    for (PointLightObject* light : context.pointLights)
    {
        const auto shadowIndex = shadowSystem.GetOrAllocatePointShadowIndex(light);
        if (!shadowIndex.has_value())
            continue;

        shadowConstants.push_back(PointLightShadowConstants{
            .range = light->GetRadius(),
            .bias = light->GetBias() });

        const Vector3 eye = light->GetPosition();
        const Matrix lightProj = Matrix::MakePerspective(
            Math::DegToRad(90.0f), 1.0f, 0.1f, light->GetRadius());

        const Vector3 lightDirections[6] = {
            Vector3::Right,
            Vector3::Right * -1.0f,
            Vector3::Forward,
            Vector3::Forward * -1.0f,
            Vector3::Up,
            Vector3::Up * -1.0f
        };

        const Vector3 lightUps[6] = {
            Vector3::Forward * -1.0f,
            Vector3::Forward * -1.0f,
            Vector3::Up,
            Vector3::Up * -1.0f,
            Vector3::Forward * -1.0f,
            Vector3::Forward * -1.0f
        };

        for (uint32 face = 0; face < 6; ++face)
        {
            const Vector3 lightDirection = lightDirections[face];
            const Vector3 up = lightUps[face];

            const Matrix lightView = Matrix::MakeView(eye, eye + lightDirection, up);

            TextureView* faceView = shadowSystem.GetPointShadowFaceView(*shadowIndex, face);

            const RenderingAttachmentInfo depthAttachment = {
                .imageView = faceView->GetHandle(),
                .layout = ImageLayout::DepthAttachment,
                .loadOp = LoadOp::Clear,
                .storeOp = StoreOp::Store,
                .clearDepth = 1.0f,
                .clearStencil = 0
            };

            const RenderingInfo renderingInfo = {
                .renderArea = {
                    .offset = { 0, 0 },
                    .extent = { width, height } },
                .layerCount = 1,
                .colorAttachments = {},
                .depthAttachment = &depthAttachment
            };

            context.commandBuffer.BeginRendering(renderingInfo);
            context.commandBuffer.SetViewport(
                0.0f, 0.0f,
                static_cast<float>(width), static_cast<float>(height),
                0.0f, 1.0f);
            context.commandBuffer.SetScissor(0, 0, width, height);

            for (const DrawCommand& cmd : drawCmds)
            {
                PipelineStateDesc psoDesc = cmd.pipelineState;
                psoDesc.shader = shadowShader;
                psoDesc.rendering = PipelineRenderingDesc{
                    .colorAttachmentFormats = {},
                    .depthAttachmentFormat = Format::D32_Float,
                    .stencilAttachmentFormat = Format::Unknown
                };

                PipelineState* pso = context.resourceManager.GetOrCreatePSO(psoDesc);

                context.commandBuffer.BindPipeline(*pso);

                ShadowMapConstants shadowMapConstants = {};
                shadowMapConstants.world = cmd.worldMatrix;
                shadowMapConstants.lightViewProj = lightView * lightProj;

                if (pso->GetLayout()->SupportsPushConstants())
                {
                    vkCmdPushConstants(
                        context.commandBuffer.GetHandle(),
                        pso->GetLayout()->GetHandle(),
                        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                        0,
                        sizeof(shadowMapConstants),
                        &shadowMapConstants);
                }

                context.commandBuffer.BindVertexBuffer(*cmd.vertexBuffer);

                if (cmd.indexBuffer)
                {
                    context.commandBuffer.BindIndexBuffer(*cmd.indexBuffer);
                    context.commandBuffer.DrawIndexed(cmd.indexCount, cmd.indexOffset);
                }
                else
                {
                    context.commandBuffer.Draw(cmd.vertexCount);
                }
            }

            context.commandBuffer.EndRendering();
        }
    }

    textureCube->Transition(context.commandBuffer, ImageLayout::DepthReadOnly);

    context.frameResource.pointLightShadowStorageBuffer->Update(shadowConstants.data(), sizeof(PointLightShadowConstants) * shadowConstants.size());
}

} // namespace URay::Render
