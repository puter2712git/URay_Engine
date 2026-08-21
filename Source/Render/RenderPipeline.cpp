#include "RenderPipeline.h"

#include "Render/Renderer.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Unit.h"

#include "Editor/Editor.h"

namespace URay::RHI
{

RenderPipeline::RenderPipeline(Renderer& renderer)
    : renderer(renderer), builder(DrawCommandBuilder(renderer))
{
}

void RenderPipeline::Reset()
{
    builder.Reset();
}

void RenderPipeline::Execute(const std::vector<Scene*>& scenes)
{
    CameraComponent* camera = FindCamera(scenes);
    if (!camera)
        return;

    Matrix viewMatrix = camera->GetViewMatrix();
    Matrix projMatrix = camera->GetProjMatrix();
    renderer.SetFrameViewInfo(viewMatrix, projMatrix);

    builder.FlushLines();
    builder.FlushTexts();

    const std::vector<DrawCommand>& cmds = builder.GetCommands();

    for (const DrawCommand& cmd : cmds)
    {
        ExecuteCommand(cmd);
    }
}

::URay::CameraComponent* RenderPipeline::FindCamera(const std::vector<Scene*>& scenes) const
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

        if (camera)
            break;
    }

    return camera;
}

void RenderPipeline::CollectCommand(const std::vector<Scene*>& scenes)
{
    for (const Scene* scene : scenes)
    {
        for (const Unit* unit : scene->GetUnits())
        {
            if (RenderComponent* comp = unit->GetComponent<RenderComponent>())
            {
                comp->SubmitCommand(builder);
            }
        }
    }
}

void RenderPipeline::ExecuteCommand(const DrawCommand& cmd) const
{
    renderer.Draw(cmd);
}

} // namespace URay::RHI
