#include "ObjectIdTargetRegistry.h"

#include <Render/Rendering/Object/RenderObject.h>
#include <Render/Rendering/Scene/SceneSystem.h>

namespace URay
{

ObjectIdTargetRegistry::ObjectIdTargetRegistry(Render::SceneSystem& sceneSystem)
    : sceneSystem(sceneSystem)
{
    sceneSystem.onObjectAdded.Register(
        this,
        [this](Render::RenderScene* scene, Render::RenderObject* object, Unit* unit, Component* component)
        {
            OnRenderObjectAdded(scene, object, unit, component);
        });
    sceneSystem.onObjectDestroyed.Register(
        this,
        [this](Render::RenderScene* scene, Render::RenderObject* object)
        {
            OnRenderObjectDestroyed(scene, object);
        });
}

ObjectIdTargetRegistry::~ObjectIdTargetRegistry()
{
    sceneSystem.onObjectDestroyed.UnregisterAll(this);
    sceneSystem.onObjectAdded.UnregisterAll(this);
}

Unit* ObjectIdTargetRegistry::FindUnit(uint32 objectId) const
{
    if (objectId == 0)
        return nullptr;

    const auto it = objectTargets.find(objectId);
    return it != objectTargets.end() ? it->second : nullptr;
}

void ObjectIdTargetRegistry::OnRenderObjectAdded(
    Render::RenderScene*,
    Render::RenderObject* object,
    Unit* unit,
    Component*)
{
    if (!object || !unit)
        return;

    objectTargets.insert_or_assign(object->GetId(), unit);
}

void ObjectIdTargetRegistry::OnRenderObjectDestroyed(
    Render::RenderScene*,
    Render::RenderObject* object)
{
    if (object)
        objectTargets.erase(object->GetId());
}

} // namespace URay
