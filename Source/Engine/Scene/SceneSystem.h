#pragma once

#include "Engine/Ray/EventRay.h"
#include "Engine/Scene/Scene.h"

#include <memory>
#include <vector>

namespace URay
{

class Component;
class Property;

class SceneSystem
{
public:
    SceneSystem();
    ~SceneSystem();

public:
    bool Initialize();
    void Finalize();

public:
    void Update(float deltaTime);

    std::unique_ptr<Scene> CreateScene(SceneType type, const VirtualPath& filePath);

    void SwitchScene(std::unique_ptr<Scene> scene);

    void LoadScene(std::unique_ptr<Scene> scene);
    void UnloadScene(SceneType type);

    Scene* GetSceneByType(SceneType type) const;

    const std::vector<std::unique_ptr<Scene>>& GetScenes() const { return scenes; }

    EventRay<Scene*>& GetSceneAddRay() { return sceneAddRay; }
    EventRay<Scene*>& GetSceneDestroyRay() { return sceneDestroyRay; }

    EventRay<Scene*, Unit*>& GetUnitAddRay() { return unitAddRay; }
    EventRay<Scene*, Unit*>& GetUnitRemoveRay() { return unitRemoveRay; }
    EventRay<Scene*, Unit*>& GetUnitWorldTransformUpdateRay() { return unitWorldTransformUpdateRay; }

    EventRay<Scene*, Unit*, Component*>& GetComponentAddRay() { return componentAddRay; }
    EventRay<Scene*, Unit*, Component*>& GetComponentDestroyRay() { return componentDestroyRay; }
    EventRay<Scene*, Unit*, Component*, const Property&>& GetComponentPropertyChangeRay() { return componentPropertyChangedRay; }

private:
    std::vector<std::unique_ptr<Scene>> scenes;

    EventRay<Scene*> sceneAddRay;
    EventRay<Scene*> sceneDestroyRay;
    EventRay<Scene*, Unit*> unitAddRay;
    EventRay<Scene*, Unit*> unitRemoveRay;
    EventRay<Scene*, Unit*> unitWorldTransformUpdateRay;
    EventRay<Scene*, Unit*, Component*> componentAddRay;
    EventRay<Scene*, Unit*, Component*> componentDestroyRay;
    EventRay<Scene*, Unit*, Component*, const Property&> componentPropertyChangedRay;
};

} // namespace URay
