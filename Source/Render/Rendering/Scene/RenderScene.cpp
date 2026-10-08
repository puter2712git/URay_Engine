#include "RenderScene.h"

#include <Engine/Component/Component.h>
#include <Engine/Spatial/Octree.h>

#include <Render/Rendering/Object/BoundedObject.h>
#include <Render/Rendering/Object/RenderObject.h>
#include <Render/Rendering/Object/ViewObject.h>
#include <Render/Rendering/Scene/SceneSystem.h>

#include <Core/Math/AABB.h>

#include <algorithm>

namespace URay::Render
{

RenderScene::RenderScene(SceneSystem& sceneSystem) : sceneSystem(sceneSystem) {}

RenderScene::~RenderScene() = default;

const std::vector<RenderObject*>& RenderScene::GetObjects(const Unit* unit) const
{
    static const std::vector<RenderObject*> emptyObjects;

    const auto it = objectsByUnit.find(unit);
    return it != objectsByUnit.end() ? it->second : emptyObjects;
}

void RenderScene::Add(std::unique_ptr<RenderObject> object, Unit* unit)
{
    if (!object)
        return;

    RenderObject* renderObject = object.get();
    renderObject->SetScene(this);

    if (ViewObject* viewObj = Cast<ViewObject>(object.get()))
    {
        viewObjects.push_back(viewObj);
    }

    if (unit)
    {
        objectsByUnit[unit].push_back(renderObject);
        unitByObject.emplace(renderObject, unit);
    }

    objects.push_back(std::move(object));
}

void RenderScene::Destroy(RenderObject* object)
{
    if (!object)
        return;

    if (ViewObject* viewObject = Cast<ViewObject>(object))
    {
        std::erase(viewObjects, viewObject);
    }

    const auto ownerIt = unitByObject.find(object);
    if (ownerIt != unitByObject.end())
    {
        const Unit* unit = ownerIt->second;
        const auto objectsIt = objectsByUnit.find(unit);
        if (objectsIt != objectsByUnit.end())
        {
            std::erase(objectsIt->second, object);
            if (objectsIt->second.empty())
            {
                objectsByUnit.erase(objectsIt);
            }
        }

        unitByObject.erase(ownerIt);
    }

    std::erase_if(objects, [&](std::unique_ptr<RenderObject>& robj)
                  { return object == robj.get(); });
}

} // namespace URay::Render
