#pragma once

#include <Core/Type/Ray/Ray.h>
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

public:
    Ray<Scene*> onSceneAdded;
    Ray<Scene*> onSceneDestroyed;
    Ray<Scene*, Unit*> onUnitAdded;
    Ray<Scene*, Unit*> onUnitRemoved;
    Ray<Scene*, Unit*> onUnitWorldTransformUpdated;
    Ray<Scene*, Unit*, Component*> onComponentAdded;
    Ray<Scene*, Unit*, Component*> onComponentDestroyed;
    Ray<Scene*, Unit*, Component*, const Property&> onComponentPropertyChanged;

private:
    SceneId nextSceneId = 1;

    std::vector<std::unique_ptr<Scene>> scenes;

};

} // namespace URay
