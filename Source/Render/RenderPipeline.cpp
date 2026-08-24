#include "RenderPipeline.h"

#include "Render/Renderer.h"
#include "Render/Scene/Object/ViewObject.h"
#include "Render/Scene/RenderScene.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Unit.h"

#include "Editor/Editor.h"

namespace URay::Render
{

RenderPipeline::RenderPipeline(Renderer& renderer)
    : renderer(renderer), builder(DrawCommandBuilder(renderer, *renderer.GetResourceManager()))
{
}

void RenderPipeline::Reset()
{
    builder.Reset();
}

void RenderPipeline::Execute(const std::vector<RenderScene*>& scenes)
{
    ViewObject* view = FindView(scenes);
    if (!view)
        return;

    const Matrix& viewMatrix = view->GetViewMatrix();
    const Matrix& projMatrix = view->GetProjMatrix();
    renderer.SetFrameViewInfo(viewMatrix, projMatrix);

    for (const RenderScene* scene : scenes)
    {
        for (size_t index = 0; index < scene->GetObjectCount(); ++index)
        {
            RenderObject* robj = scene->GetObject(index);
            if (robj)
            {
                robj->Submit(builder);
            }
        }
    }

    builder.FlushLines();
    builder.FlushTexts();

    const std::vector<DrawCommand>& cmds = builder.GetCommands();

    {
        URAY_PROFILE_SCOPE("RenderPipeline::ExecuteDrawCommands")
        for (const DrawCommand& cmd : cmds)
        {
            ExecuteCommand(cmd);
        }
    }
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

void RenderPipeline::ExecuteCommand(const DrawCommand& cmd) const
{
    renderer.Draw(cmd);
}

} // namespace URay::Render
