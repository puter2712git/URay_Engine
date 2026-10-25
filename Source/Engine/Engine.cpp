#include "Engine.h"
#include <Core/Type/Types.h>

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/UI/UISystem.h>

#include <Core/Input/Action/InputActionSystem.h>
#include <Core/Input/InputSystem.h>
#include <Core/Log/FileLogSink.h>
#include <Core/Log/LogSystem.h>
#include <Core/Log/StdErrLogSink.h>
#include <Core/Performance/PerformanceAnalytics.h>
#include <Core/TimeFlow/Schedule/Scheduler.h>
#include <Core/TimeFlow/Timer.h>

#include <Platform/Input/GLFWInputAdapter.h>
#include <Platform/Window/Window.h>

#include <Render/RenderSystem.h>
#include <Render/Rendering/RenderPass/UIPass.h>
#include <Render/Rendering/RenderPipeline.h>
#include <Render/Rendering/Renderer.h>

#include <GLFW/glfw3.h>

#include <algorithm>

namespace URay
{

Engine* gEngine;

Engine::Engine() = default;

Engine::~Engine() = default;

bool Engine::Initialize(
    const String& enginePath,
    const String& projectPath)
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

    timer = std::make_unique<Timer>();
    scheduler = std::make_unique<Scheduler>();

    inputSystem = std::make_unique<InputSystem>();
    inputActionSystem = std::make_unique<InputActionSystem>();

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

    uiSystem = std::make_unique<UISystem>(*window, *inputSystem);
    uiSystem->Initialize();
    renderSystem->GetPipeline().AddRenderPass(std::make_unique<Render::UIPass>());

    assetSystem = std::make_unique<AssetSystem>();
    if (!assetSystem->Initialize(enginePath, projectPath))
    {
        URAY_LOG("[Engine] Failed to initialize asset system.");
        Finalize();
        return false;
    }

    window->onKey.Register(this, [this](KeyCode key, KeyAction action, ModifierKey mods)
                           { inputSystem->OnKey(key, action, mods); });
    window->onMouseButton.Register(this, [this](MouseButton button, KeyAction action, ModifierKey mods)
                                   { inputSystem->OnMouseButton(button, action, mods); });
    window->onCursorMoved.Register(this, [this](Vector2 position)
                                   { inputSystem->OnCursorMoved(position); });
    window->onTextInput.Register(this, [this](char32_t codepoint)
                                 { inputSystem->OnTextInput(codepoint); });
    window->onScroll.Register(this, [this](Vector2 delta)
                              { inputSystem->OnScroll(delta); });
    window->onFocusChanged.Register(this, [this](bool focused)
                                    { inputSystem->OnWindowFocusChanged(focused); });

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

    if (uiSystem)
    {
        uiSystem->Finalize();
        uiSystem.reset();
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

    if (assetSystem)
    {
        assetSystem->Finalize();
        assetSystem.reset();
    }

    if (performanceAnalytics)
    {
        performanceAnalytics.reset();
    }

    if (inputActionSystem)
    {
        inputActionSystem.reset();
    }

    if (inputSystem)
    {
        inputSystem.reset();
    }

    if (timer)
    {
        timer.reset();
    }

    if (window)
    {
        window->onKey.UnregisterAll(this);
        window->onMouseButton.UnregisterAll(this);
        window->onCursorMoved.UnregisterAll(this);
        window->onTextInput.UnregisterAll(this);
        window->onScroll.UnregisterAll(this);
        window->onFocusChanged.UnregisterAll(this);

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

    inputSystem->BeginFrame();
    glfwPollEvents();
    inputSystem->PublishSnapshot(++frameIndex);

    inputActionSystem->Update(inputSystem->GetSnapshot());

    timer->Tick();
    scheduler->Update(timer->GetDeltaTime());

    uiSystem->Update(timer->GetDeltaTime());

    sceneSystem->Update(timer->GetDeltaTime());
}

void Engine::BeginRender()
{
    URAY_PROFILE_SCOPE("Engine::BeginRender");

    renderSystem->GetPipeline().Reset();
    renderSystem->BeginFrame();

    uiSystem->PrepareRender();
    uiSystem->Paint();
}

void Engine::Render(const Render::RenderRequest& request)
{
    URAY_PROFILE_SCOPE("Engine::Render");

    renderSystem->GetPipeline().Execute(request);
    renderSystem->EndFrame();
}

} // namespace URay
