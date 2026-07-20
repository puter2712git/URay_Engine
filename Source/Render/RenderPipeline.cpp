#include "RenderPipeline.h"

#include "Render/Renderer.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/Unit.h"

#include "Editor/Editor.h"

namespace URay
{

RenderPipeline::RenderPipeline(Renderer& renderer)
    : renderer(renderer), builder(DrawCommandBuilder(renderer))
{
}

void RenderPipeline::Execute(const Scene* scene)
{
    CameraComponent* camera = FindCamera(scene);
    if (!camera)
        return;

    builder.Reset();

    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix projMatrix = camera->GetProjMatrix();
    renderer.SetFrameViewInfo(viewMatrix, projMatrix);

    CollectCommand(scene);

    std::vector<DrawCommand> cmds = builder.GetCommands();

    renderer.BeginFrame();

    for (const DrawCommand& cmd : cmds)
    {
        ExecuteCommand(cmd);
    }

    gEngine->GetEditor()->Render();

    renderer.EndFrame();
}

CameraComponent* RenderPipeline::FindCamera(const Scene* scene) const
{
    CameraComponent* camera = nullptr;

    for (const Unit* unit : scene->GetUnits())
    {
        if (CameraComponent* cam = unit->GetComponent<CameraComponent>())
        {
            camera = cam;
            break;
        }
    }

    return camera;
}

void RenderPipeline::CollectCommand(const Scene* scene)
{
    for (const Unit* unit : scene->GetUnits())
    {
        if (RenderComponent* comp = unit->GetComponent<RenderComponent>())
        {
            comp->SubmitCommand(builder);
        }
    }
}

void RenderPipeline::ExecuteCommand(const DrawCommand& cmd) const
{
    renderer.Draw(cmd);
}

} // namespace URay
