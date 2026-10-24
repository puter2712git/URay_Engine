#include "RenderObject.h"

#include <Render/Rendering/Scene/RenderScene.h>
#include <Render/Rendering/Scene/SceneSystem.h>

namespace URay::Render
{

void RenderObject::NotifyUpdated()
{
    if (!scene)
        return;

    SceneSystem& sceneSystem = scene->GetSceneSystem();
    sceneSystem.onObjectUpdated.Emit(scene, this);
}

} // namespace URay::Render
