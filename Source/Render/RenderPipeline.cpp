#include "RenderPipeline.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/RenderPass/FogPass.h"
#include "Render/RenderPass/OpaquePass.h"
#include "Render/RenderPass/OverlayPass.h"
#include "Render/RenderSystem.h"
#include "Render/Renderer.h"
#include "Render/Scene/Object/BoundedObject.h"
#include "Render/Scene/Object/Drawable/DrawableObject.h"
#include "Render/Scene/Object/FogObject.h"
#include "Render/Scene/Object/RenderObject.h"
#include "Render/Scene/Object/ViewObject.h"
#include "Render/Scene/RenderScene.h"

#include "Core/Math/Frustum.h"

#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Unit.h"
#include "Engine/Spatial/Octree.h"

#include "Editor/Editor.h"

namespace URay::Render
{

RenderPipeline::RenderPipeline(RenderSystem& renderSystem)
    : renderSystem(renderSystem)
{
}

RenderPipeline::~RenderPipeline() = default;

bool RenderPipeline::Initialize()
{
    builder = std::make_unique<DrawCommandBuilder>(renderSystem);
    if (!builder->Initialize())
        return false;

    passes.push_back(std::make_unique<OpaquePass>());
    passes.push_back(std::make_unique<OverlayPass>());
    passes.push_back(std::make_unique<FogPass>(renderSystem));

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

void RenderPipeline::Execute(const std::vector<RenderScene*>& scenes)
{
    ViewObject* view = FindView(scenes);
    if (!view)
        return;

    FogObject* fog = FindFog(scenes);

    Renderer& renderer = renderSystem.GetRenderer();

    const Matrix& viewMatrix = view->GetViewMatrix();
    const Matrix& projMatrix = view->GetProjMatrix();
    renderer.SetFrameViewInfo(viewMatrix, projMatrix);

    const Frustum frustum =
        Frustum::FromViewProjection(viewMatrix * projMatrix);

    for (const RenderScene* scene : scenes)
    {
        for (RenderObject* object : scene->GetUnboundedObjects())
        {
            if (DrawableObject* drawableObject =
                    dynamic_cast<DrawableObject*>(object))
            {
                drawableObject->Submit(*builder);
            }
        }

        std::vector<BoundedObject*> visibleObjects;
        scene->GetOctree()->Query(frustum, visibleObjects);

        for (BoundedObject* boundedObject : visibleObjects)
        {
            if (DrawableObject* drawableObject =
                    dynamic_cast<DrawableObject*>(boundedObject))
            {
                drawableObject->Submit(*builder);
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

ViewObject* RenderPipeline::FindView(const std::vector<RenderScene*>& scenes) const
{
    for (const RenderScene* scene : scenes)
    {
        ViewObject* view = scene->GetView();
        if (view)
        {
            return view;
        }
    }

    return nullptr;
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

} // namespace URay::Render
