#include "RenderObject.h"

#include "Render/Rendering/Scene/RenderScene.h"
#include "Render/Rendering/Scene/SceneSystem.h"

namespace URay::Render
{

void RenderObject::NotifyUpdated()
{
    if (!scene)
        return;

    SceneSystem& sceneSystem = scene->GetSceneSystem();
    sceneSystem.GetObjectUpdateRay().Emit(scene, this);
}

} // namespace URay::Render
