#pragma once

#include <Core/Ray/EventRay.h>
#include <Engine/Scene/Scene.h>

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

    Scene* LoadScene(std::unique_ptr<Scene> scene);
    bool UnloadScene(SceneId id);

    Scene* GetScene(SceneId id) const;
    std::vector<Scene*> GetScenes(SceneType type) const;

    const std::vector<std::unique_ptr<Scene>>& GetScenes() const { return scenes; }

    EventRay<Scene*>& GetSceneAddRay() { return onSceneAdded; }
    EventRay<Scene*>& GetSceneDestroyRay() { return onSceneDestroyed; }

    EventRay<Scene*, Unit*>& GetUnitAddRay() { return onUnitAdded; }
    EventRay<Scene*, Unit*>& GetUnitRemoveRay() { return onUnitRemoved; }
    EventRay<Scene*, Unit*>& GetUnitWorldTransformUpdateRay() { return onUnitWorldTransformUpdated; }

    EventRay<Scene*, Unit*, Component*>& GetComponentAddRay() { return onComponentAdded; }
    EventRay<Scene*, Unit*, Component*>& GetComponentDestroyRay() { return onComponentDestroyed; }
    EventRay<Scene*, Unit*, Component*, const Property&>& GetComponentPropertyChangeRay() { return onComponentPropertyChanged; }

private:
    SceneId nextSceneId = 1;

    std::vector<std::unique_ptr<Scene>> scenes;

    EventRay<Scene*> onSceneAdded;
    EventRay<Scene*> onSceneDestroyed;
    EventRay<Scene*, Unit*> onUnitAdded;
    EventRay<Scene*, Unit*> onUnitRemoved;
    EventRay<Scene*, Unit*> onUnitWorldTransformUpdated;
    EventRay<Scene*, Unit*, Component*> onComponentAdded;
    EventRay<Scene*, Unit*, Component*> onComponentDestroyed;
    EventRay<Scene*, Unit*, Component*, const Property&> onComponentPropertyChanged;
};

} // namespace URay
