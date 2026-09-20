#include "Render/Rendering/RenderPipeline.h"

#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RenderSystem.h"
#include "Render/Rendering/DrawCommand/DrawCommandBuilder.h"
#include "Render/Rendering/Object/BoundedObject.h"
#include "Render/Rendering/Object/DecalObject.h"
#include "Render/Rendering/Object/Drawable/DrawableObject.h"
#include "Render/Rendering/Object/Drawable/MeshObject.h"
#include "Render/Rendering/Object/FogObject.h"
#include "Render/Rendering/Object/Light/AmbientLightObject.h"
#include "Render/Rendering/Object/Light/DirectionalLightObject.h"
#include "Render/Rendering/Object/Light/PointLightObject.h"
#include "Render/Rendering/Object/Light/SpotLightObject.h"
#include "Render/Rendering/Object/RenderObject.h"
#include "Render/Rendering/Object/ViewObject.h"
#include "Render/Rendering/RenderConstants.h"
#include "Render/Rendering/RenderPass/DecalPass.h"
#include "Render/Rendering/RenderPass/FogPass.h"
#include "Render/Rendering/RenderPass/OpaquePass.h"
#include "Render/Rendering/RenderPass/OverlayPass.h"
#include "Render/Rendering/RenderPass/ShadowPass.h"
#include "Render/Rendering/Renderer.h"
#include "Render/Rendering/Scene/RenderScene.h"

#include "Core/Math/Frustum.h"
#include "Core/Math/Math.h"

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

RenderPipeline::RenderPipeline(RenderSystem& renderSystem)
    : assetSystem(gEngine->GetAssetSystem()), renderSystem(renderSystem) {}

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

    passes.push_back(std::make_unique<ShadowPass>());
    passes.push_back(std::make_unique<OpaquePass>());
    passes.push_back(std::make_unique<DecalPass>());
    passes.push_back(std::make_unique<OverlayPass>());
    passes.push_back(std::make_unique<FogPass>(renderSystem, fogShader));

    return true;
}

void RenderPipeline::Finalize()
{
    builder->Finalize();
    builder.reset();
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
    DirectionalLightObject* directionalLight = FindDirectionalLight(request.scenes);
    std::vector<PointLightObject*> pointLights = FindPointLights(request.scenes);
    std::vector<SpotLightObject*> spotLights = FindSpotLights(request.scenes);

    Renderer& renderer = renderSystem.GetRenderer();

    RenderView view = request.view;

    builder->SetViewMode(view.viewMode);

    FrameConstants frameConstants = {};
    frameConstants.view = view.viewMatrix;
    frameConstants.invView = view.viewMatrix.Inverse();
    frameConstants.proj = view.projMatrix;
    frameConstants.invProj = view.projMatrix.Inverse();
    frameConstants.viewProj = view.viewMatrix * view.projMatrix;
    frameConstants.invViewProj = frameConstants.viewProj.Inverse();
    frameConstants.nearPlane = view.nearPlane;
    frameConstants.farPlane = view.farPlane;
    frameConstants.renderTargetSize = Vector2(
        renderer.GetSceneRenderTarget().GetExtent().width,
        renderer.GetSceneRenderTarget().GetExtent().height);

    if (ambientLight)
    {
        frameConstants.ambientLight.intensity = ambientLight->GetIntensity();
        frameConstants.ambientLight.color = Color3(ambientLight->GetColor());
    }

    if (directionalLight)
    {
        frameConstants.directionalLight.direction = directionalLight->GetDirection();
        frameConstants.directionalLight.intensity = directionalLight->GetIntensity();
        frameConstants.directionalLight.color = directionalLight->GetColor();
        frameConstants.directionalLight.castShadow = directionalLight->CastsShadow();
    }

    std::vector<PointLightConstants> pointLightConstants;
    uint32 shadowIndex = 0;
    for (size_t i = 0; i < pointLights.size(); ++i)
    {
        pointLightConstants.push_back(PointLightConstants{
            .position = pointLights[i]->GetPosition(),
            .radius = pointLights[i]->GetRadius(),
            .intensity = pointLights[i]->GetIntensity(),
            .color = Color3(pointLights[i]->GetColor()),
            .shadowIndex = pointLights[i]->CastsShadow() ? shadowIndex++ : UINT_MAX });
    }

    std::vector<SpotLightConstants> spotLightConstants;
    shadowIndex = 0;
    for (size_t i = 0; i < spotLights.size(); ++i)
    {
        spotLightConstants.push_back(SpotLightConstants{
            .position = spotLights[i]->GetPosition(),
            .range = spotLights[i]->GetRange(),
            .direction = spotLights[i]->GetDirection(),
            .intensity = spotLights[i]->GetIntensity(),
            .color = Color3(spotLights[i]->GetColor()),
            .innerConeAngle = Math::DegToRad(spotLights[i]->GetInnerConeAngle() * 0.5f),
            .outerConeAngle = Math::DegToRad(spotLights[i]->GetOuterConeAngle() * 0.5f),
            .shadowIndex = spotLights[i]->CastsShadow() ? shadowIndex++ : UINT_MAX });
    }

    renderer.GetFrameResource().uniformBuffer->Update(&frameConstants, sizeof(FrameConstants));
    renderer.GetFrameResource().pointLightStorageBuffer->Update(pointLightConstants.data(), sizeof(PointLightConstants) * pointLightConstants.size());
    renderer.GetFrameResource().spotLightStorageBuffer->Update(spotLightConstants.data(), sizeof(SpotLightConstants) * spotLightConstants.size());

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
                if (!meshData)
                    continue;

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
        .commandBuffer = *renderer.GetFrameResource().commandBuffer,
        .resourceManager = renderSystem.GetResourceManager(),
        .renderView = view,

        .frameResource = renderer.GetFrameResource(),

        .sceneRenderTarget = renderer.GetSceneRenderTarget(),
        .postProcessRenderTarget = renderer.GetPostProcessRenderTarget(),

        .swapChainImage = renderer.GetSwapChainImage(),
        .swapChainImageView = renderer.GetSwapChainImageView(),
        .swapChainExtent = renderer.GetSwapChainExtent(),

        .fogObject = fog,
        .directionalLight = directionalLight,
        .spotLights = spotLights,
        .pointLights = pointLights
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

DirectionalLightObject* RenderPipeline::FindDirectionalLight(
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

std::vector<SpotLightObject*> RenderPipeline::FindSpotLights(
    const std::vector<RenderScene*>& scenes) const
{
    std::vector<SpotLightObject*> spotLights;

    for (const RenderScene* scene : scenes)
    {
        size_t objCount = scene->GetObjectCount();

        for (size_t i = 0; i < objCount; ++i)
        {
            RenderObject* robj = scene->GetObject(i);

            if (SpotLightObject* light = dynamic_cast<SpotLightObject*>(robj))
            {
                spotLights.push_back(light);
            }
        }
    }

    return spotLights;
}

} // namespace URay::Render
