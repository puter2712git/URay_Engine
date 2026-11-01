#pragma once

#include <Core/Type/TypeAlias.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Engine/Scene/Scene.h>

#include <memory>
#include <vector>

namespace URay
{

class Component;
class Property;

class SceneSystem : public RayEmitter
{
public:
    SceneSystem();
    ~SceneSystem();

public:
    bool Initialize();
    void Finalize();

public:
    void Update(float deltaTime);

    UniquePtr<Scene> CreateScene(SceneType type, const VirtualPath& filePath);

    Scene* LoadScene(UniquePtr<Scene> scene);
    bool UnloadScene(SceneId id);

    Scene* GetScene(SceneId id) const;
    std::vector<Scene*> GetScenes(SceneType type) const;

    const std::vector<UniquePtr<Scene>>& GetScenes() const { return scenes; }

private:
    SceneId nextSceneId = 1;

    std::vector<UniquePtr<Scene>> scenes;
};

} // namespace URay
