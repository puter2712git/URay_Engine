#pragma once

#include <Core/Performance/ScopeTimer.h>
#include <Core/Type/TypeAlias.h>

#include <functional>
#include <vector>

namespace URay
{

class WindowSystem;
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
class ScriptSystem;
class Window;
struct WindowDesc;

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

    bool BeginRender();
    void Render(const Render::RenderRequest& request);

    using AdditionalWindowRenderRequestBuilder =
        std::function<void(Window&, Render::RenderRequest&)>;

    void RenderAdditionalWindows(
        const AdditionalWindowRenderRequestBuilder& buildRequest = {});

    Window* CreateAdditionalWindow(const WindowDesc& desc);
    bool DestroyAdditionalWindow(Window& window);
    bool RequestCloseAdditionalWindow(Window& window);
    void ProcessAdditionalWindowCloseRequests();

    LogSystem& GetLogSystem() const { return *logSystem; }
    WindowSystem& GetWindowSystem() const { return *windowSystem; }
    Timer& GetTimer() const { return *timer; }
    Scheduler& GetScheduler() const { return *scheduler; }
    InputSystem& GetInputSystem() const { return *inputSystem; }
    InputActionSystem& GetInputActionSystem() const { return *inputActionSystem; }
    UISystem& GetUISystem() const { return *uiSystem; }
    PerformanceAnalytics& GetPerformanceAnalytics() const { return *performanceAnalytics; }

    ObjectSystem& GetObjectSystem() const { return *objectSystem; }
    AssetSystem& GetAssetSystem() const { return *assetSystem; }
    SceneSystem& GetSceneSystem() const { return *sceneSystem; }
    ScriptSystem& GetScriptSystem() const { return *scriptSystem; }
    Render::RenderSystem& GetRenderSystem() const { return *renderSystem; }

private:
    uint64 frameIndex = 0;

    UniquePtr<LogSystem> logSystem = nullptr;
    UniquePtr<StdErrLogSink> stdErrLogSink = nullptr;
    UniquePtr<FileLogSink> fileLogSink = nullptr;

    UniquePtr<WindowSystem> windowSystem = nullptr;
    UniquePtr<Timer> timer = nullptr;
    UniquePtr<Scheduler> scheduler = nullptr;

    UniquePtr<InputSystem> inputSystem = nullptr;
    UniquePtr<InputActionSystem> inputActionSystem = nullptr;
    UniquePtr<UISystem> uiSystem = nullptr;
    UniquePtr<PerformanceAnalytics> performanceAnalytics = nullptr;

    UniquePtr<ObjectSystem> objectSystem = nullptr;
    UniquePtr<AssetSystem> assetSystem = nullptr;
    UniquePtr<SceneSystem> sceneSystem = nullptr;
    UniquePtr<ScriptSystem> scriptSystem = nullptr;

    UniquePtr<Render::RenderSystem> renderSystem = nullptr;

    Vector<Window*> additionalWindows;
};

extern Engine* gEngine;

#define URAY_PROFILE_SCOPE(name) \
    ::URay::ScopeTimer _urayScopeTimer(gEngine->GetPerformanceAnalytics(), name);

} // namespace URay
