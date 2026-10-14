#pragma once

namespace URay
{

class SceneSystem;
class Scene;

class PlaySession
{
public:
    explicit PlaySession(SceneSystem& sceneSystem);

public:
    bool Start();
    void Stop();

    bool IsPlaying() const { return playScene != nullptr; }
    Scene* GetScene() const { return playScene; }

private:
    SceneSystem& sceneSystem;
    Scene* playScene = nullptr;
};

} // namespace URay