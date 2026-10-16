#include "PlaySession.h"

#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SceneSystem.h>

namespace URay
{

PlaySession::PlaySession(SceneSystem& sceneSystem) : sceneSystem(sceneSystem) {}

bool PlaySession::Start()
{
    if (IsPlaying())
        return false;

    auto scenes = sceneSystem.GetScenes(SceneType::Game);
    Scene* gameScene = scenes.empty() ? nullptr : scenes[0];

    if (!gameScene)
        return false;

    YAML::Node gameSceneNode = gameScene->Serialize();

    auto newPlayScene = sceneSystem.CreateScene(SceneType::Play, "");
    newPlayScene->Deserialize(gameSceneNode);

    playScene = sceneSystem.LoadScene(std::move(newPlayScene));

    return true;
}

void PlaySession::Stop()
{
    if (!IsPlaying())
        return;

    sceneSystem.UnloadScene(playScene->GetId());
    playScene = nullptr;
}

} // namespace URay
