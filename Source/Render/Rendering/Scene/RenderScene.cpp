#include "RenderScene.h"

#include <Engine/Component/Component.h>
#include <Engine/Spatial/Octree.h>

#include <Render/Rendering/Object/BoundedObject.h>
#include <Render/Rendering/Object/FogObject.h>
#include <Render/Rendering/Object/Light/AmbientLightObject.h>
#include <Render/Rendering/Object/Light/DirectionalLightObject.h>
#include <Render/Rendering/Object/Light/PointLightObject.h>
#include <Render/Rendering/Object/Light/SpotLightObject.h>
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

void RenderScene::AddToTypeIndexes(RenderObject* object)
{
    if (ViewObject* viewObject = Cast<ViewObject>(object))
        viewObjects.push_back(viewObject);

    if (FogObject* fogObject = Cast<FogObject>(object))
        fogObjects.push_back(fogObject);

    if (AmbientLightObject* ambientLightObject = Cast<AmbientLightObject>(object))
        ambientLightObjects.push_back(ambientLightObject);

    if (DirectionalLightObject* directionalLightObject = Cast<DirectionalLightObject>(object))
        directionalLightObjects.push_back(directionalLightObject);

    if (PointLightObject* pointLightObject = Cast<PointLightObject>(object))
        pointLightObjects.push_back(pointLightObject);

    if (SpotLightObject* spotLightObject = Cast<SpotLightObject>(object))
        spotLightObjects.push_back(spotLightObject);
}

void RenderScene::RemoveFromTypeIndexes(RenderObject* object)
{
    if (ViewObject* viewObject = Cast<ViewObject>(object))
        std::erase(viewObjects, viewObject);

    if (FogObject* fogObject = Cast<FogObject>(object))
        std::erase(fogObjects, fogObject);

    if (AmbientLightObject* ambientLightObject = Cast<AmbientLightObject>(object))
        std::erase(ambientLightObjects, ambientLightObject);

    if (DirectionalLightObject* directionalLightObject = Cast<DirectionalLightObject>(object))
        std::erase(directionalLightObjects, directionalLightObject);

    if (PointLightObject* pointLightObject = Cast<PointLightObject>(object))
        std::erase(pointLightObjects, pointLightObject);

    if (SpotLightObject* spotLightObject = Cast<SpotLightObject>(object))
        std::erase(spotLightObjects, spotLightObject);
}

void RenderScene::Add(std::unique_ptr<RenderObject> object, Unit* unit)
{
    if (!object)
        return;

    RenderObject* renderObject = object.get();
    renderObject->SetScene(this);
    AddToTypeIndexes(renderObject);

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

    RemoveFromTypeIndexes(object);

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
