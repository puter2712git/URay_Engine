#include "ObjectIdTargetRegistry.h"

#include <Engine/Component/Component.h>
#include <Engine/Scene/Unit.h>
#include <Render/Rendering/Object/RenderObject.h>
#include <Render/Rendering/Scene/RenderScene.h>
#include <Render/Rendering/Scene/SceneSystem.h>

namespace URay
{

ObjectIdTargetRegistry::ObjectIdTargetRegistry(Render::SceneSystem& sceneSystem)
    : sceneSystem(sceneSystem)
{
    sceneSystem.Register("ObjectAdded", this, &ObjectIdTargetRegistry::OnRenderObjectAdded);
    sceneSystem.Register("ObjectDestroyed", this, &ObjectIdTargetRegistry::OnRenderObjectDestroyed);
}

ObjectIdTargetRegistry::~ObjectIdTargetRegistry()
{
    sceneSystem.UnregisterAll(this);
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
