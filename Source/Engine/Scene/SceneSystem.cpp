#include "SceneSystem.h"

namespace URay
{

bool SceneSystem::Initialize()
{
    return true;
}

void SceneSystem::Finalize()
{
}

void SceneSystem::Update(float deltaTime)
{
    for (const auto& scene : scenes)
    {
        scene->Update(deltaTime);
    }
}

void SceneSystem::SwitchScene(std::unique_ptr<Scene> scene)
{
    if (!scene)
        return;

    UnloadScene(scene->GetType());
    LoadScene(std::move(scene));
}

void SceneSystem::LoadScene(std::unique_ptr<Scene> scene)
{
    if (!scene)
        return;

    scenes.push_back(std::move(scene));
}

void SceneSystem::UnloadScene(SceneType type)
{
    Scene* targetScene = GetSceneByType(type);
    if (targetScene)
    {
        scenes.erase(std::remove_if(scenes.begin(), scenes.end(),
                                    [targetScene](const std::unique_ptr<Scene>& scene)
                                    {
                                        return scene.get() == targetScene;
                                    }),
                     scenes.end());
    }
}

Scene* SceneSystem::GetSceneByType(SceneType type) const
{
    for (const auto& scene : scenes)
    {
        if (scene->GetType() == type)
        {
            return scene.get();
        }
    }

    return nullptr;
}

} // namespace URay
