#include <Render/Rendering/RenderPipeline.h>

#include <Render/RHI/Buffer/Buffer.h>
#include <Render/RHI/CommandBuffer/CommandBuffer.h>
#include <Render/RHI/RenderTarget.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/Batch/UIDrawContext.h>
#include <Render/Rendering/DrawCommand/DrawCommandBuilder.h>
#include <Render/Rendering/Font/FontSystem.h>
#include <Render/Rendering/Object/BoundedObject.h>
#include <Render/Rendering/Object/DecalObject.h>
#include <Render/Rendering/Object/Drawable/DrawableObject.h>
#include <Render/Rendering/Object/Drawable/MeshObject.h>
#include <Render/Rendering/Object/FogObject.h>
#include <Render/Rendering/Object/Light/AmbientLightObject.h>
#include <Render/Rendering/Object/Light/DirectionalLightObject.h>
#include <Render/Rendering/Object/Light/PointLightObject.h>
#include <Render/Rendering/Object/Light/SpotLightObject.h>
#include <Render/Rendering/Object/RenderObject.h>
#include <Render/Rendering/Object/ViewObject.h>
#include <Render/Rendering/RenderConstants.h>
#include <Render/Rendering/RenderPass/DecalPass.h>
#include <Render/Rendering/RenderPass/FogPass.h>
#include <Render/Rendering/RenderPass/GizmoPass.h>
#include <Render/Rendering/RenderPass/OpaquePass.h>
#include <Render/Rendering/RenderPass/OverlayPass.h>
#include <Render/Rendering/RenderPass/SelectionMaskPass.h>
#include <Render/Rendering/RenderPass/SelectionOutlinePass.h>
#include <Render/Rendering/RenderPass/ShadowPass.h>
#include <Render/Rendering/Renderer.h>
#include <Render/Rendering/Scene/RenderEnvironment.h>
#include <Render/Rendering/Scene/RenderScene.h>
#include <Render/ResourceManager.h>

#include <Core/Math/Frustum.h>
#include <Core/Math/Math.h>

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/Unit.h>
#include <Engine/Spatial/Octree.h>

#include <Editor/Editor.h>

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

    passes.push_back(std::make_unique<ShadowPass>());
    passes.push_back(std::make_unique<OpaquePass>());
    passes.push_back(std::make_unique<DecalPass>());
    passes.push_back(std::make_unique<OverlayPass>());
    passes.push_back(std::make_unique<SelectionMaskPass>());
    passes.push_back(std::make_unique<FogPass>());
    passes.push_back(std::make_unique<SelectionOutlinePass>());
    passes.push_back(std::make_unique<GizmoPass>());

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
    for (const SceneRenderInfo& sceneInfo : request.sceneRenderInfos)
    {
        Reset();
        ExecuteSceneRenderInfo(sceneInfo);
    }

    if (!request.uiDrawContext)
        return;

    Reset();

    RenderView uiView = request.sceneRenderInfos.empty()
                            ? RenderView{}
                            : request.sceneRenderInfos.back().view;
    ExecuteUI(*request.uiDrawContext, uiView);
}

void RenderPipeline::ExecuteSceneRenderInfo(const SceneRenderInfo& sceneInfo)
{
    if (!sceneInfo.targets.IsValid())
        return;

    FogObject* fog = nullptr;
    AmbientLightObject* ambientLight = nullptr;
    DirectionalLightObject* directionalLight = nullptr;
    std::vector<PointLightObject*> pointLights;
    std::vector<SpotLightObject*> spotLights;

    for (RenderScene* scene : sceneInfo.scenes)
    {
        if (!scene)
            continue;

        const RenderEnvironment& environment = scene->GetEnvironment();

        if (!fog)
            fog = environment.GetFog();

        if (!ambientLight)
            ambientLight = environment.GetAmbientLight();

        if (!directionalLight)
            directionalLight = environment.GetDirectionalLight();

        const std::vector<PointLightObject*>& scenePointLights = scene->GetPointLights();
        pointLights.insert(pointLights.end(), scenePointLights.begin(), scenePointLights.end());

        const std::vector<SpotLightObject*>& sceneSpotLights = scene->GetSpotLights();
        spotLights.insert(spotLights.end(), sceneSpotLights.begin(), sceneSpotLights.end());
    }

    Renderer& renderer = renderSystem.GetRenderer();

    const Extent2D extent = sceneInfo.targets.scene->GetExtent();

    RenderView view = sceneInfo.view;

    const DrawCommandParams drawCommandParams = {
        .viewMode = view.viewMode
    };

    FrameConstants frameConstants = {};
    frameConstants.view = view.viewMatrix;
    frameConstants.invView = view.viewMatrix.Inverse();
    frameConstants.proj = view.projMatrix;
    frameConstants.invProj = view.projMatrix.Inverse();
    frameConstants.viewProj = view.viewMatrix * view.projMatrix;
    frameConstants.invViewProj = frameConstants.viewProj.Inverse();
    frameConstants.nearPlane = view.nearPlane;
    frameConstants.farPlane = view.farPlane;
    frameConstants.renderTargetSize = Vector2(static_cast<float>(extent.width), static_cast<float>(extent.height));

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

    frameConstants.pointLightCount = static_cast<uint32>(pointLights.size());
    frameConstants.spotLightCount = static_cast<uint32>(spotLights.size());

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

    for (const RenderScene* scene : sceneInfo.scenes)
    {
        size_t objectCount = scene->GetObjectCount();

        std::vector<MeshObject*> meshReceivers;
        std::vector<DecalObject*> decals;

        for (size_t i = 0; i < objectCount; ++i)
        {
            RenderObject* robj = scene->GetObject(i);

            if (!robj->IsEnabled())
                continue;

            if (MeshObject* mesh = Cast<MeshObject>(robj))
            {
                meshReceivers.push_back(mesh);
            }

            if (DecalObject* decal = Cast<DecalObject>(robj))
            {
                decals.push_back(decal);
            }

            if (DrawableObject* drawableObj = Cast<DrawableObject>(robj))
            {
                drawableObj->Submit(*builder, drawCommandParams);
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

    FontSystem& fontSystem = renderSystem.GetResourceManager().GetFontSystem();
    if (!fontSystem.FlushAtlasUploads())
    {
    }

    for (Unit* selectedUnit : sceneInfo.selectedUnits)
    {
        for (RenderScene* scene : sceneInfo.scenes)
        {
            if (!scene)
                continue;

            for (RenderObject* renderObject : scene->GetObjects(selectedUnit))
            {
                if (!renderObject || !renderObject->IsEnabled())
                    continue;

                if (DrawableObject* drawableObject = Cast<DrawableObject>(renderObject))
                {
                    drawableObject->SubmitSelectionMask(*builder, drawCommandParams);
                }
            }
        }
    }

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
        .sceneTargets = &sceneInfo.targets,

        .swapChainImage = renderer.GetSwapChainImage(),
        .swapChainImageView = renderer.GetSwapChainImageView(),
        .swapChainExtent = renderer.GetSwapChainExtent(),

        .fogObject = fog,
        .directionalLight = directionalLight,
        .spotLights = spotLights,
        .pointLights = pointLights
    };

    const VkCommandBuffer vkCommandBuffer = passContext.commandBuffer.GetHandle();
    const VkQueryPool queryPool = passContext.frameResource.gpuTimeStamps.queryPool;

    for (auto& pass : passes)
    {
        RenderPassId passId = pass->GetPassId();

        if (passId == RenderPassId::UI)
            continue;

        const uint32 passIndex = static_cast<uint32>(passId);

        const uint32 beginQuery = passIndex * 2;
        const uint32 endQuery = beginQuery + 1;

        vkCmdWriteTimestamp2(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            queryPool,
            beginQuery);

        const size_t passIdIndex = static_cast<size_t>(passId);

        pass->Begin(passContext);
        pass->Execute(passContext, drawCmds[passIdIndex]);
        pass->End(passContext);

        vkCmdWriteTimestamp2(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            queryPool,
            endQuery);
    }

    Texture* idTexture = sceneInfo.targets.scene->GetColorTexture(1);
    renderer.GetObjectIdReadback().Record(
        passContext.commandBuffer,
        *idTexture,
        currentFrame);
}

void RenderPipeline::ExecuteUI(const UIDrawContext& context, RenderView& view)
{
    builder->FlushUI(context);

    for (const DrawCommand& cmd : builder->GetCommands())
    {
        const size_t passIdIndex = static_cast<size_t>(cmd.passId);
        drawCmds[passIdIndex].push_back(cmd);
    }

    Renderer& renderer = renderSystem.GetRenderer();
    std::vector<SpotLightObject*> spotLights;
    std::vector<PointLightObject*> pointLights;

    const RenderPassContext passContext = {
        .commandBuffer = *renderer.GetFrameResource().commandBuffer,
        .resourceManager = renderSystem.GetResourceManager(),
        .renderView = view,

        .frameResource = renderer.GetFrameResource(),
        .sceneTargets = nullptr,

        .swapChainImage = renderer.GetSwapChainImage(),
        .swapChainImageView = renderer.GetSwapChainImageView(),
        .swapChainExtent = renderer.GetSwapChainExtent(),

        .fogObject = nullptr,
        .directionalLight = nullptr,
        .spotLights = spotLights,
        .pointLights = pointLights
    };

    const VkCommandBuffer vkCommandBuffer = passContext.commandBuffer.GetHandle();
    const VkQueryPool queryPool = passContext.frameResource.gpuTimeStamps.queryPool;

    for (auto& pass : passes)
    {
        if (pass->GetPassId() != RenderPassId::UI)
            continue;

        const uint32 passIndex = static_cast<uint32>(RenderPassId::UI);
        const uint32 beginQuery = passIndex * 2;
        const uint32 endQuery = beginQuery + 1;

        vkCmdWriteTimestamp2(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            queryPool,
            beginQuery);

        pass->Begin(passContext);
        pass->Execute(passContext, drawCmds[passIndex]);
        pass->End(passContext);

        vkCmdWriteTimestamp2(
            vkCommandBuffer,
            VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            queryPool,
            endQuery);
    }
}

void RenderPipeline::AddRenderPass(std::unique_ptr<RenderPass> pass)
{
    passes.push_back(std::move(pass));
}

} // namespace URay::Render
