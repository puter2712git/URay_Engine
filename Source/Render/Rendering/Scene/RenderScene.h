#pragma once

#include <Core/Type/Type.h>
#include <Render/Rendering/Scene/RenderEnvironment.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace URay
{
class Unit;
}

namespace URay::Render
{

class SceneSystem;
class RenderObject;
class ViewObject;
class PointLightObject;
class SpotLightObject;

class RenderScene
{
    URAY_PLAIN_CLASS(RenderScene)

    friend class SceneSystem;

public:
    RenderScene(SceneSystem& sceneSystem);
    ~RenderScene();

public:
    SceneSystem& GetSceneSystem() { return sceneSystem; }

    size_t GetObjectCount() const { return objects.size(); }
    RenderObject* GetObject(size_t index) const { return objects[index].get(); }
    const std::vector<RenderObject*>& GetObjects(const Unit* unit) const;

    ViewObject* GetView() const { return viewObjects.empty() ? nullptr : viewObjects[0]; }
    RenderEnvironment& GetEnvironment() { return environment; }
    const RenderEnvironment& GetEnvironment() const { return environment; }
    const std::vector<PointLightObject*>& GetPointLights() const { return pointLightObjects; }
    const std::vector<SpotLightObject*>& GetSpotLights() const { return spotLightObjects; }

private:
    void Add(std::unique_ptr<RenderObject> object, Unit* unit);
    void Destroy(RenderObject* object);
    void AddToTypeIndexes(RenderObject* object);
    void RemoveFromTypeIndexes(RenderObject* object);

private:
    SceneSystem& sceneSystem;

    RenderEnvironment environment = {};

    std::vector<std::unique_ptr<RenderObject>> objects;
    std::vector<ViewObject*> viewObjects;
    std::vector<PointLightObject*> pointLightObjects;
    std::vector<SpotLightObject*> spotLightObjects;

    std::unordered_map<const Unit*, std::vector<RenderObject*>> objectsByUnit;
    std::unordered_map<RenderObject*, const Unit*> unitByObject;
};

} // namespace URay::Render
