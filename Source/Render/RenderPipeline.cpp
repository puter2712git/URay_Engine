#include "RenderPipeline.h"

#include "Render/Renderer.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/Render/GizmoComponent.h"
#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Unit.h"

#include "Editor/Editor.h"

namespace URay
{

RenderPipeline::RenderPipeline(Renderer& renderer)
    : renderer(renderer), builder(DrawCommandBuilder(renderer))
{
}

void RenderPipeline::Execute(const std::vector<Scene*>& scenes)
{
    CameraComponent* camera = FindCamera(scenes);
    if (!camera)
        return;

    builder.Reset();

    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix projMatrix = camera->GetProjMatrix();
    renderer.SetFrameViewInfo(viewMatrix, projMatrix);

    CollectCommand(scenes);
    builder.FlushLines();

    std::vector<DrawCommand> cmds = builder.GetCommands();

    if (!renderer.BeginFrame())
        return;

    for (const DrawCommand& cmd : cmds)
    {
        ExecuteCommand(cmd);
    }

    gEngine->GetEditor()->Render();

    renderer.EndFrame();
}

CameraComponent* RenderPipeline::FindCamera(const std::vector<Scene*> scenes) const
{
    CameraComponent* camera = nullptr;

    for (const Scene* scene : scenes)
    {
        for (const Unit* unit : scene->GetUnits())
        {
            if (CameraComponent* cam = unit->GetComponent<CameraComponent>())
            {
                camera = cam;
                break;
            }
        }

        if (!camera)
            break;
    }

    return camera;
}

void RenderPipeline::CollectCommand(const std::vector<Scene*> scenes)
{
    GizmoComponent* gizmo = nullptr;

    for (const Scene* scene : scenes)
    {
        for (const Unit* unit : scene->GetUnits())
        {
            if (RenderComponent* comp = unit->GetComponent<RenderComponent>())
            {
                if (GizmoComponent* gizmoComp = dynamic_cast<GizmoComponent*>(comp))
                {
                    gizmo = gizmoComp;
                }
                else
                {
                    comp->SubmitCommand(builder);
                }
            }
        }
    }

    if (gizmo)
    {
        gizmo->SubmitCommand(builder);
    }
}

void RenderPipeline::ExecuteCommand(const DrawCommand& cmd) const
{
    renderer.Draw(cmd);
}

} // namespace URay
