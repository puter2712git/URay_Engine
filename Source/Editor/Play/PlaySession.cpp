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

    Scene* gameScene = sceneSystem.GetSceneByType(SceneType::Game);
    if (!gameScene)
        return false;

    YAML::Node gameSceneNode = gameScene->Serialize();

    auto newPlayScene = sceneSystem.CreateScene(SceneType::Play, "");
    newPlayScene->Deserialize(gameSceneNode);

    playScene = newPlayScene.get();

    sceneSystem.LoadScene(std::move(newPlayScene));

    return true;
}

void PlaySession::Stop()
{
    if (!IsPlaying())
        return;

    sceneSystem.UnloadScene(SceneType::Play);
    playScene = nullptr;
}

} // namespace URay
