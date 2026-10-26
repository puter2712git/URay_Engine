#pragma once
#include <Core/Type/Types.h>

#include <Core/Performance/ScopeTimer.h>

#include <vector>

namespace URay
{

class Window;
class Timer;
class Scheduler;
class LogSystem;
class InputSystem;
class InputActionSystem;
class UISystem;
class PerformanceAnalytics;
class StdErrLogSink;
class FileLogSink;

class ObjectSystem;
class AssetSystem;
class SceneSystem;

namespace Render
{
class RenderSystem;
struct RenderRequest;
} // namespace Render

class Engine
{
public:
    Engine();
    ~Engine();

public:
    bool Initialize(const String& enginePath, const String& projectPath);
    void Finalize();

    void Update();

    void BeginRender();
    void Render(const Render::RenderRequest& request);

    LogSystem& GetLogSystem() const { return *logSystem; }
    Window& GetWindow() const { return *window; }
    Timer& GetTimer() const { return *timer; }
    Scheduler& GetScheduler() const { return *scheduler; }
    InputSystem& GetInputSystem() const { return *inputSystem; }
    InputActionSystem& GetInputActionSystem() const { return *inputActionSystem; }
    UISystem& GetUISystem() const { return *uiSystem; }
    PerformanceAnalytics& GetPerformanceAnalytics() const { return *performanceAnalytics; }

    ObjectSystem& GetObjectSystem() const { return *objectSystem; }
    AssetSystem& GetAssetSystem() const { return *assetSystem; }
    SceneSystem& GetSceneSystem() const { return *sceneSystem; }
    Render::RenderSystem& GetRenderSystem() const { return *renderSystem; }

private:
    uint64 frameIndex = 0;

    std::unique_ptr<LogSystem> logSystem = nullptr;
    std::unique_ptr<StdErrLogSink> stdErrLogSink = nullptr;
    std::unique_ptr<FileLogSink> fileLogSink = nullptr;

    std::unique_ptr<Window> window = nullptr;
    std::unique_ptr<Timer> timer = nullptr;
    std::unique_ptr<Scheduler> scheduler = nullptr;

    std::unique_ptr<InputSystem> inputSystem = nullptr;
    std::unique_ptr<InputActionSystem> inputActionSystem = nullptr;
    std::unique_ptr<UISystem> uiSystem = nullptr;
    std::unique_ptr<PerformanceAnalytics> performanceAnalytics = nullptr;

    std::unique_ptr<ObjectSystem> objectSystem = nullptr;
    std::unique_ptr<AssetSystem> assetSystem = nullptr;
    std::unique_ptr<SceneSystem> sceneSystem = nullptr;
    std::unique_ptr<Render::RenderSystem> renderSystem = nullptr;
};

extern Engine* gEngine;

#define URAY_PROFILE_SCOPE(name) \
    ::URay::ScopeTimer _urayScopeTimer(gEngine->GetPerformanceAnalytics(), name);

} // namespace URay
