#include "Engine.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Scene/SceneSystem.h"

#include "Core/Performance/PerformanceAnalytics.h"
#include "Core/Timer.h"

#include "Platform/Input/GLFWInputAdapter.h"
#include "Platform/Window/Window.h"

#include "Render/RenderPipeline.h"
#include "Render/RenderSystem.h"
#include "Render/Renderer.h"

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

    window = std::make_unique<Window>();
    if (!window->Initialize())
        return false;

    glfwSetKeyCallback(window->GetGLFWWindow(), KeyCallback);
    glfwSetMouseButtonCallback(window->GetGLFWWindow(), MouseButtonCallback);
    glfwSetCursorPosCallback(window->GetGLFWWindow(), CursorPosCallback);

    timer = std::make_unique<Timer>();
    inputManager = std::make_unique<InputManager>();
    performanceAnalytics = std::make_unique<PerformanceAnalytics>();

    assetSystem = std::make_unique<AssetSystem>();
    if (!assetSystem->Initialize(enginePath, projectPath))
        return false;

    renderSystem = std::make_unique<Render::RenderSystem>();
    if (!renderSystem->Initialize(*window, assetSystem->GetFilesystem()))
        return false;

    if (!assetSystem->InitializeRuntimeAssets(
            renderSystem->GetDevice(),
            renderSystem->GetResourceManager(),
            renderSystem->GetShaderManager()))
    {
        return false;
    }

    sceneSystem = std::make_unique<SceneSystem>();

    return true;
}

void Engine::Finalize()
{
    renderSystem->WaitIdle();

    if (sceneSystem)
    {
        sceneSystem.reset();
    }

    if (renderSystem)
    {
        renderSystem->Finalize();
        renderSystem.reset();
    }

    if (assetSystem)
    {
        assetSystem->Finalize();
        assetSystem.reset();
    }

    if (performanceAnalytics)
    {
        performanceAnalytics.reset();
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

void Engine::Render()
{
    URAY_PROFILE_SCOPE("Engine::Render");

    std::vector<Render::RenderScene*> renderScenes;
    for (const auto& scene : sceneSystem->GetScenes())
    {
        renderScenes.push_back(scene->GetRenderScene());
    }

    Render::Renderer& renderer = renderSystem->GetRenderer();

    renderSystem->GetPipeline().Execute(renderScenes);
    renderer.EndFrame();
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
