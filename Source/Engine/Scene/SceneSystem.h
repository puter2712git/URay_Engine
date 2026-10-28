#pragma once

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

    std::unique_ptr<Scene> CreateScene(SceneType type, const VirtualPath& filePath);

    Scene* LoadScene(std::unique_ptr<Scene> scene);
    bool UnloadScene(SceneId id);

    Scene* GetScene(SceneId id) const;
    std::vector<Scene*> GetScenes(SceneType type) const;

    const std::vector<std::unique_ptr<Scene>>& GetScenes() const { return scenes; }

private:
    SceneId nextSceneId = 1;

    std::vector<std::unique_ptr<Scene>> scenes;
};

} // namespace URay
