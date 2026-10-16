#pragma once

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
class FogObject;
class AmbientLightObject;
class DirectionalLightObject;
class PointLightObject;
class SpotLightObject;

class RenderScene
{
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
    FogObject* GetFog() const { return fogObjects.empty() ? nullptr : fogObjects[0]; }
    AmbientLightObject* GetAmbientLight() const { return ambientLightObjects.empty() ? nullptr : ambientLightObjects[0]; }
    DirectionalLightObject* GetDirectionalLight() const { return directionalLightObjects.empty() ? nullptr : directionalLightObjects[0]; }
    const std::vector<PointLightObject*>& GetPointLights() const { return pointLightObjects; }
    const std::vector<SpotLightObject*>& GetSpotLights() const { return spotLightObjects; }

private:
    void Add(std::unique_ptr<RenderObject> object, Unit* unit);
    void Destroy(RenderObject* object);
    void AddToTypeIndexes(RenderObject* object);
    void RemoveFromTypeIndexes(RenderObject* object);

private:
    SceneSystem& sceneSystem;

    std::vector<std::unique_ptr<RenderObject>> objects;
    std::vector<ViewObject*> viewObjects;
    std::vector<FogObject*> fogObjects;
    std::vector<AmbientLightObject*> ambientLightObjects;
    std::vector<DirectionalLightObject*> directionalLightObjects;
    std::vector<PointLightObject*> pointLightObjects;
    std::vector<SpotLightObject*> spotLightObjects;

    std::unordered_map<const Unit*, std::vector<RenderObject*>> objectsByUnit;
    std::unordered_map<RenderObject*, const Unit*> unitByObject;
};

} // namespace URay::Render
