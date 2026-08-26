#pragma once

#include "Core/Input/InputManager.h"
#include "Core/Performance/ScopeTimer.h"

#include <string>
#include <vector>

namespace URay
{

class Window;
class Timer;
class PerformanceAnalytics;

class AssetSystem;
class SceneSystem;

namespace Render
{
class RenderSystem;
} // namespace Render

class Engine
{
public:
    Engine();
    ~Engine();

public:
    bool Initialize(const std::string& projectPath);
    void Finalize();

    void Update();

    void BeginRender();
    void PrepareRender();
    void Render();
    void EndRender();

    Window& GetWindow() const { return *window; }
    Timer& GetTimer() const { return *timer; }
    InputManager& GetInputManager() { return *inputManager; }
    PerformanceAnalytics& GetPerformanceAnalytics() const { return *performanceAnalytics; }

    AssetSystem& GetAssetSystem() const { return *assetSystem; }
    SceneSystem& GetSceneSystem() const { return *sceneSystem; }
    Render::RenderSystem& GetRenderSystem() const { return *renderSystem; }

private:
    std::unique_ptr<Window> window = nullptr;
    std::unique_ptr<Timer> timer = nullptr;
    std::unique_ptr<InputManager> inputManager = nullptr;
    std::unique_ptr<PerformanceAnalytics> performanceAnalytics = nullptr;

    std::unique_ptr<AssetSystem> assetSystem = nullptr;
    std::unique_ptr<SceneSystem> sceneSystem = nullptr;
    std::unique_ptr<Render::RenderSystem> renderSystem = nullptr;
};

extern Engine* gEngine;

#define URAY_PROFILE_SCOPE(name) \
    ::URay::ScopeTimer _urayScopeTimer(gEngine->GetPerformanceAnalytics(), name);

} // namespace URay
