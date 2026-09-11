#include "RenderPipeline.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RenderConstants.h"
#include "Render/RenderPass/DecalPass.h"
#include "Render/RenderPass/FogPass.h"
#include "Render/RenderPass/OpaquePass.h"
#include "Render/RenderPass/OverlayPass.h"
#include "Render/RenderSystem.h"
#include "Render/Renderer.h"
#include "Render/Scene/Object/BoundedObject.h"
#include "Render/Scene/Object/DecalObject.h"
#include "Render/Scene/Object/Drawable/DrawableObject.h"
#include "Render/Scene/Object/Drawable/MeshObject.h"
#include "Render/Scene/Object/FogObject.h"
#include "Render/Scene/Object/Light/AmbientLightObject.h"
#include "Render/Scene/Object/Light/DirectionalLightObject.h"
#include "Render/Scene/Object/Light/PointLightObject.h"
#include "Render/Scene/Object/RenderObject.h"
#include "Render/Scene/Object/ViewObject.h"
#include "Render/Scene/RenderScene.h"

#include "Core/Math/Frustum.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Unit.h"
#include "Engine/Spatial/Octree.h"

#include "Editor/Editor.h"

namespace URay::Render
{

RenderPipeline::RenderPipeline(AssetSystem& assetSystem, RenderSystem& renderSystem)
    : assetSystem(assetSystem), renderSystem(renderSystem)
{
}

RenderPipeline::~RenderPipeline() = default;

bool RenderPipeline::Initialize()
{
    builder = std::make_unique<DrawCommandBuilder>(assetSystem, renderSystem);
    if (!builder->Initialize())
        return false;

    std::vector<URay::Shader*> shaders = assetSystem.FindAssets<URay::Shader>();

    URay::Shader* fogShader = nullptr;

    for (URay::Shader* shader : shaders)
    {
        if (shader->GetName() == "Fog")
        {
            fogShader = shader;
            break;
        }
    }

    passes.push_back(std::make_unique<OpaquePass>());
    passes.push_back(std::make_unique<DecalPass>());
    passes.push_back(std::make_unique<OverlayPass>());
    passes.push_back(std::make_unique<FogPass>(renderSystem, fogShader));

    return true;
}

void RenderPipeline::Finalize()
{
    if (builder)
    {
        builder->Finalize();
        builder.reset();
    }
}

void RenderPipeline::Reset()
{
    builder->Reset();

    for (auto& cmds : drawCmds)
    {
        cmds.clear();
    }
}

void RenderPipeline::Execute(const RenderRequest& request)
{
    FogObject* fog = FindFog(request.scenes);
    AmbientLightObject* ambientLight = FindAmbientLight(request.scenes);
    DirectionalLightObject* light = FindLight(request.scenes);
    std::vector<PointLightObject*> pointLights = FindPointLights(request.scenes);

    Renderer& renderer = renderSystem.GetRenderer();

    const RenderView& view = request.view;

    builder->SetViewMode(view.viewMode);

    FrameConstants frameConstants = {};
    frameConstants.view = view.viewMatrix;
    frameConstants.invView = view.viewMatrix.Inverse();
    frameConstants.proj = view.projMatrix;
    frameConstants.invProj = view.projMatrix.Inverse();
    frameConstants.viewProj = view.viewMatrix * view.projMatrix;
    frameConstants.invViewProj = frameConstants.viewProj.Inverse();
    frameConstants.nearPlane = 0.1f;
    frameConstants.farPlane = 1000.0f;
    frameConstants.renderTargetSize = Vector2(
        renderer.GetSceneRenderTarget().GetExtent().width,
        renderer.GetSceneRenderTarget().GetExtent().height);

    if (ambientLight)
    {
        frameConstants.ambientLight.intensity = ambientLight->GetIntensity();
        frameConstants.ambientLight.color = Color3(ambientLight->GetColor());
    }

    if (light)
    {
        frameConstants.directionalLight.direction = light->GetDirection();
        frameConstants.directionalLight.intensity = light->GetIntensity();
        frameConstants.directionalLight.color = light->GetColor();
    }

    std::vector<PointLightConstants> pointLightConstants;
    for (size_t i = 0; i < pointLights.size(); ++i)
    {
        pointLightConstants.push_back(PointLightConstants{
            .position = pointLights[i]->GetPosition(),
            .radius = pointLights[i]->GetRadius(),
            .intensity = pointLights[i]->GetIntensity(),
            .color = Color3(pointLights[i]->GetColor()) });
    }

    renderer.GetFrameUniformBuffer(currentFrame)->Update(&frameConstants, sizeof(FrameConstants));
    renderer.GetPointLightStorageBuffer(currentFrame)->Update(pointLightConstants.data(), sizeof(PointLightConstants) * pointLightConstants.size());

    const Frustum frustum =
        Frustum::FromViewProjection(view.viewMatrix * view.projMatrix);

    for (const RenderScene* scene : request.scenes)
    {
        size_t objectCount = scene->GetObjectCount();

        std::vector<MeshObject*> meshReceivers;
        std::vector<DecalObject*> decals;

        for (size_t i = 0; i < objectCount; ++i)
        {
            RenderObject* robj = scene->GetObject(i);

            if (!robj->IsEnabled())
                continue;

            if (MeshObject* mesh = dynamic_cast<MeshObject*>(robj))
            {
                meshReceivers.push_back(mesh);
            }

            if (DecalObject* decal = dynamic_cast<DecalObject*>(robj))
            {
                decals.push_back(decal);
            }

            if (DrawableObject* drawableObj = dynamic_cast<DrawableObject*>(robj))
            {
                drawableObj->Submit(*builder);
            }
        }

        for (DecalObject* decal : decals)
        {
            for (MeshObject* mesh : meshReceivers)
            {
                if (!decal->GetWorldBounds().Intersects(mesh->GetWorldBounds()))
                    continue;

                Mesh* meshData = mesh->GetMesh();

                for (const auto& section : meshData->GetSections())
                {
                    builder->BuildDecal(
                        { .meshWorldMatrix = mesh->GetWorldMatrix(),
                          .receiverMesh = mesh->GetMesh(),
                          .indexOffset = section.indexOffset,
                          .indexCount = section.indexCount,
                          .decalMaterial = decal->GetMaterial(),
                          .decalDescriptorSet = decal->GetDescriptorSet(currentFrame) });
                }
            }
        }
    }

    builder->FlushLines();
    builder->FlushTexts();

    const std::vector<DrawCommand>& cmds = builder->GetCommands();

    for (const DrawCommand& cmd : cmds)
    {
        RenderPassId passId = cmd.passId;
        size_t passIdIndex = static_cast<size_t>(passId);

        drawCmds[passIdIndex].push_back(cmd);
    }

    const RenderPassContext passContext = {
        .commandBuffer = renderer.GetCommandBuffer(),
        .resourceManager = renderSystem.GetResourceManager(),
        .frameDescriptorSet = renderer.GetFrameDescriptorSet(),

        .sceneRenderTarget = renderer.GetSceneRenderTarget(),
        .sceneRenderPass = renderer.GetSceneRenderPass(),
        .sceneFramebuffer = renderer.GetSceneFramebuffer(),

        .postProcessRenderTarget = renderer.GetPostProcessRenderTarget(),
        .postProcessRenderPass = renderer.GetPostProcessRenderPass(),
        .postProcessFramebuffer = renderer.GetPostProcessFramebuffer(),

        .swapChainRenderPass = renderer.GetSwapChainRenderPass(),
        .swapChainFramebuffer = renderer.GetSwapChainFramebuffer(),
        .swapChainExtent = renderer.GetSwapChainExtent(),

        .fogObject = fog
    };

    for (auto& pass : passes)
    {
        RenderPassId passId = pass->GetPassId();
        size_t passIdIndex = static_cast<size_t>(passId);

        pass->Begin(passContext);
        pass->Execute(passContext, drawCmds[passIdIndex]);
        pass->End(passContext);
    }
}

void RenderPipeline::AddRenderPass(std::unique_ptr<RenderPass> pass)
{
    passes.push_back(std::move(pass));
}

FogObject* RenderPipeline::FindFog(
    const std::vector<RenderScene*>& scenes) const
{
    for (const RenderScene* scene : scenes)
    {
        size_t objCount = scene->GetObjectCount();

        for (size_t i = 0; i < objCount; ++i)
        {
            RenderObject* robj = scene->GetObject(i);

            if (FogObject* fog = dynamic_cast<FogObject*>(robj))
            {
                return fog;
            }
        }
    }

    return nullptr;
}

AmbientLightObject* RenderPipeline::FindAmbientLight(
    const std::vector<RenderScene*>& scenes) const
{
    for (const RenderScene* scene : scenes)
    {
        size_t objCount = scene->GetObjectCount();

        for (size_t i = 0; i < objCount; ++i)
        {
            RenderObject* robj = scene->GetObject(i);

            if (AmbientLightObject* light = dynamic_cast<AmbientLightObject*>(robj))
            {
                return light;
            }
        }
    }

    return nullptr;
}

DirectionalLightObject* RenderPipeline::FindLight(
    const std::vector<RenderScene*>& scenes) const
{
    for (const RenderScene* scene : scenes)
    {
        size_t objCount = scene->GetObjectCount();

        for (size_t i = 0; i < objCount; ++i)
        {
            RenderObject* robj = scene->GetObject(i);

            if (DirectionalLightObject* light = dynamic_cast<DirectionalLightObject*>(robj))
            {
                return light;
            }
        }
    }

    return nullptr;
}

std::vector<PointLightObject*> RenderPipeline::FindPointLights(
    const std::vector<RenderScene*>& scenes) const
{
    std::vector<PointLightObject*> pointLights;

    for (const RenderScene* scene : scenes)
    {
        size_t objCount = scene->GetObjectCount();

        for (size_t i = 0; i < objCount; ++i)
        {
            RenderObject* robj = scene->GetObject(i);

            if (PointLightObject* light = dynamic_cast<PointLightObject*>(robj))
            {
                pointLights.push_back(light);
            }
        }
    }

    return pointLights;
}

} // namespace URay::Render
