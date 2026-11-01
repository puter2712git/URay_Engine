#pragma once

#include <Core/Type/TypeAlias.h>
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
    const Vector<RenderObject*>& GetObjects(const Unit* unit) const;

    ViewObject* GetView() const { return viewObjects.empty() ? nullptr : viewObjects[0]; }
    RenderEnvironment& GetEnvironment() { return environment; }
    const RenderEnvironment& GetEnvironment() const { return environment; }
    const Vector<PointLightObject*>& GetPointLights() const { return pointLightObjects; }
    const Vector<SpotLightObject*>& GetSpotLights() const { return spotLightObjects; }

private:
    void Add(UniquePtr<RenderObject> object, Unit* unit);
    void Destroy(RenderObject* object);
    void AddToTypeIndexes(RenderObject* object);
    void RemoveFromTypeIndexes(RenderObject* object);

private:
    SceneSystem& sceneSystem;

    RenderEnvironment environment = {};

    Vector<UniquePtr<RenderObject>> objects;
    Vector<ViewObject*> viewObjects;
    Vector<PointLightObject*> pointLightObjects;
    Vector<SpotLightObject*> spotLightObjects;

    std::unordered_map<const Unit*, Vector<RenderObject*>> objectsByUnit;
    std::unordered_map<RenderObject*, const Unit*> unitByObject;
};

} // namespace URay::Render
