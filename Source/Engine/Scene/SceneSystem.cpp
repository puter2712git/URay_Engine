#include "SceneSystem.h"

#include <Engine/Scene/Unit.h>

#include <cassert>

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
    while (!scenes.empty())
    {
        UnloadScene(scenes.back()->GetId());
    }
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
    assert(nextSceneId != InvalidSceneId);

    const SceneId id = nextSceneId++;
    std::unique_ptr<Scene> newScene = std::make_unique<Scene>(*this, id, type, filePath);
    return newScene;
}

Scene* SceneSystem::LoadScene(std::unique_ptr<Scene> scene)
{
    if (!scene)
        return nullptr;

    Scene* scenePtr = scene.get();

    scenes.push_back(std::move(scene));
    onSceneAdded.Emit(scenePtr);

    for (auto& unit : scenePtr->GetUnits())
    {
        onUnitAdded.Emit(scenePtr, unit.get());

        for (auto& component : unit->GetComponents())
        {
            onComponentAdded.Emit(scenePtr, unit.get(), component.get());
        }
    }

    if (scenePtr->GetSceneType() == SceneType::Play)
    {
        scenePtr->Load();
    }

    return scenePtr;
}

bool SceneSystem::UnloadScene(SceneId id)
{
    Scene* targetScene = GetScene(id);

    if (!targetScene)
        return false;

    if (targetScene->GetSceneType() == SceneType::Play)
    {
        targetScene->Unload();
    }

    targetScene->DestroyAllUnits();
    onSceneDestroyed.Emit(targetScene);
    scenes.erase(std::remove_if(scenes.begin(), scenes.end(),
                                [targetScene](const std::unique_ptr<Scene>& scene)
                                {
                                    return scene.get() == targetScene;
                                }),
                 scenes.end());

    return true;
}

Scene* SceneSystem::GetScene(SceneId id) const
{
    for (const auto& scene : scenes)
    {
        if (scene->GetId() == id)
        {
            return scene.get();
        }
    }

    return nullptr;
}

std::vector<Scene*> SceneSystem::GetScenes(SceneType type) const
{
    std::vector<Scene*> matchingScenes;

    for (const auto& scene : scenes)
    {
        if (scene->GetSceneType() == type)
        {
            matchingScenes.push_back(scene.get());
        }
    }

    return matchingScenes;
}

} // namespace URay
