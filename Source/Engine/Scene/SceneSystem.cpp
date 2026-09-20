#include "SceneSystem.h"

#include "Engine/Scene/Unit.h"

namespace URay
{

SceneSystem::SceneSystem() = default;

SceneSystem::~SceneSystem() = default;

bool SceneSystem::Initialize()
{
    return true;
}

void SceneSystem::Finalize()
{
    scenes.clear();
}

void SceneSystem::Update(float deltaTime)
{
    for (const auto& scene : scenes)
    {
        scene->Update(deltaTime);
    }
}

std::unique_ptr<Scene> SceneSystem::CreateScene(SceneType type, const VirtualPath& filePath)
{
    std::unique_ptr<Scene> newScene = std::make_unique<Scene>(*this, type, filePath);
    return newScene;
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

    Scene* scenePtr = scene.get();

    scenes.push_back(std::move(scene));
    sceneAddRay.Emit(scenePtr);

    for (auto& unit : scenePtr->GetUnits())
    {
        unitAddRay.Emit(scenePtr, unit.get());

        for (auto& component : unit->GetComponents())
        {
            componentAddRay.Emit(scenePtr, unit.get(), component.get());
        }
    }
}

void SceneSystem::UnloadScene(SceneType type)
{
    Scene* targetScene = GetSceneByType(type);

    if (targetScene)
    {
        sceneDestroyRay.Emit(targetScene);
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
