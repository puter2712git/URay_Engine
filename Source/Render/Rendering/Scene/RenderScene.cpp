#include "RenderScene.h"

#include "Engine/Component/Component.h"
#include "Engine/Spatial/Octree.h"

#include "Render/Rendering/Object/BoundedObject.h"
#include "Render/Rendering/Object/RenderObject.h"
#include "Render/Rendering/Object/ViewObject.h"
#include "Render/Rendering/Scene/SceneSystem.h"

#include "Core/Math/AABB.h"

namespace URay::Render
{

RenderScene::RenderScene(SceneSystem& sceneSystem) : sceneSystem(sceneSystem) {}

RenderScene::~RenderScene() = default;

void RenderScene::Add(std::unique_ptr<RenderObject> object, Component* component)
{
    if (!object)
        return;

    RenderObject* renderObject = object.get();
    renderObject->SetScene(this);

    if (ViewObject* viewObj = dynamic_cast<ViewObject*>(object.get()))
    {
        viewObjects.push_back(viewObj);
    }

    objects.push_back(std::move(object));

    Unit* unit = component ? component->GetOwner() : nullptr;
    sceneSystem.GetObjectAddRay().Emit(this, renderObject, unit, component);
}

void RenderScene::Destroy(RenderObject* object)
{
    if (!object)
        return;

    sceneSystem.GetObjectDestroyRay().Emit(this, object);

    std::erase_if(objects, [&](std::unique_ptr<RenderObject>& robj)
                  { return object == robj.get(); });
}

} // namespace URay::Render
