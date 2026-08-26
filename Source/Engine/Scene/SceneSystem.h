#pragma once

#include "Engine/Scene/Scene.h"

#include <memory>
#include <vector>

namespace URay
{

class SceneSystem
{
public:
    void Update(float deltaTime);

    void SwitchScene(std::unique_ptr<Scene> scene);

    void LoadScene(std::unique_ptr<Scene> scene);
    void UnloadScene(SceneType type);

    Scene* GetSceneByType(SceneType type) const;

    const std::vector<std::unique_ptr<Scene>>& GetScenes() const { return scenes; }

private:
    std::vector<std::unique_ptr<Scene>> scenes;
};

} // namespace URay
