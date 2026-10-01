#include "Engine.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/FileLogSink.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Script/ScriptSystem.h"
#include "Engine/StdErrLogSink.h"

#include "Core/Input/InputManager.h"
#include "Core/Log/LogSystem.h"
#include "Core/Performance/PerformanceAnalytics.h"
#include "Core/Timer.h"

#include "Platform/Input/GLFWInputAdapter.h"
#include "Platform/Window/Window.h"

#include "Render/RenderSystem.h"
#include "Render/Rendering/RenderPipeline.h"
#include "Render/Rendering/Renderer.h"

#include <GLFW/glfw3.h>

#include <algorithm>

namespace URay
{

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

Engine* gEngine;

Engine::Engine() = default;

Engine::~Engine() = default;

bool Engine::Initialize(
    const std::string& enginePath,
    const std::string& projectPath)
{
    gEngine = this;

    stdErrLogSink = std::make_unique<StdErrLogSink>();
    fileLogSink = std::make_unique<FileLogSink>(std::filesystem::path(projectPath) / "Log/Log.txt");

    logSystem = std::make_unique<LogSystem>();
    logSystem->RegisterSink(stdErrLogSink.get());
    logSystem->RegisterSink(fileLogSink.get());

    window = std::make_unique<Window>();
    if (!window->Initialize())
    {
        URAY_LOG("[Engine] Failed to initialize window.");
        Finalize();
        return false;
    }

    glfwSetKeyCallback(window->GetGLFWWindow(), KeyCallback);
    glfwSetMouseButtonCallback(window->GetGLFWWindow(), MouseButtonCallback);
    glfwSetCursorPosCallback(window->GetGLFWWindow(), CursorPosCallback);

    timer = std::make_unique<Timer>();

    inputManager = std::make_unique<InputManager>();
    performanceAnalytics = std::make_unique<PerformanceAnalytics>();

    sceneSystem = std::make_unique<SceneSystem>();
    if (!sceneSystem->Initialize())
    {
        URAY_LOG("[Engine] Failed to initialize scene system.");
        Finalize();
        return false;
    }

    renderSystem = std::make_unique<Render::RenderSystem>();
    if (!renderSystem->Initialize())
    {
        URAY_LOG("[Engine] Failed to initialize render system.");
        Finalize();
        return false;
    }

    assetSystem = std::make_unique<AssetSystem>();
    if (!assetSystem->Initialize(enginePath, projectPath))
    {
        URAY_LOG("[Engine] Failed to initialize asset system.");
        Finalize();
        return false;
    }

    scriptSystem = std::make_unique<ScriptSystem>();
    if (!scriptSystem->Initialize(projectPath))
    {
        URAY_LOG("[Engine] Failed to initialize script system.");
        Finalize();
        return false;
    }

    std::vector<Material*> materials = assetSystem->GetDatabase().GetAssets<Material>();
    for (Material* material : materials)
    {
        if (!material->Initialize())
        {
            URAY_LOG("[Engine] Failed to initialize material: %s", material->GetName().c_str());
            Finalize();
            return false;
        }
    }

    return true;
}

void Engine::Finalize()
{
    if (renderSystem)
    {
        renderSystem->WaitIdle();
    }

    if (scriptSystem)
    {
        scriptSystem->Finalize();
        scriptSystem.reset();
    }

    if (assetSystem)
    {
        assetSystem->Finalize();
        assetSystem.reset();
    }

    if (renderSystem)
    {
        renderSystem->Finalize();
        renderSystem.reset();
    }

    if (sceneSystem)
    {
        sceneSystem->Finalize();
        sceneSystem.reset();
    }

    if (performanceAnalytics)
    {
        performanceAnalytics.reset();
    }

    if (inputManager)
    {
        inputManager.reset();
    }

    if (timer)
    {
        timer.reset();
    }

    if (window)
    {
        window->Finalize();
        window.reset();
    }

    if (logSystem)
    {
        if (fileLogSink)
        {
            logSystem->UnregisterSink(fileLogSink.get());
            fileLogSink.reset();
        }
        if (stdErrLogSink)
        {
            logSystem->UnregisterSink(stdErrLogSink.get());
            stdErrLogSink.reset();
        }

        logSystem.reset();
    }

    gEngine = nullptr;
}

void Engine::Update()
{
    URAY_PROFILE_SCOPE("Engine::Update");

    inputManager->ClearEvents();
    inputManager->Update();

    glfwPollEvents();

    timer->Tick();

    sceneSystem->Update(timer->GetDeltaTime());
}

void Engine::BeginRender()
{
    URAY_PROFILE_SCOPE("Engine::BeginRender");

    renderSystem->GetPipeline().Reset();
    renderSystem->BeginFrame();
}

void Engine::Render(const Render::RenderRequest& request)
{
    URAY_PROFILE_SCOPE("Engine::Render");

    renderSystem->GetPipeline().Execute(request);
    renderSystem->EndFrame();
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const KeyCode keyCode = Platform::ToKeyCode(key);
    const std::optional<KeyAction> keyAction = Platform::ToKeyAction(action);

    if (keyCode == KeyCode::Unknown || !keyAction)
        return;

    gEngine->GetInputManager().OnKey(
        keyCode,
        *keyAction,
        Platform::ToModifierKey(mods));
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    const MouseButton mouseButton = Platform::ToMouseButton(button);
    const std::optional<KeyAction> mouseAction = Platform::ToKeyAction(action);

    if (mouseButton == MouseButton::None || !mouseAction)
        return;

    gEngine->GetInputManager().OnMouseButton(
        mouseButton,
        *mouseAction,
        Platform::ToModifierKey(mods));
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    gEngine->GetInputManager().OnCursorMoved(Vector2(xpos, ypos));
}

} // namespace URay
