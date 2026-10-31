#include "Engine.h"

#include <Core/Input/Action/InputActionSystem.h>
#include <Core/Input/InputSystem.h>
#include <Core/Log/FileLogSink.h>
#include <Core/Log/LogSystem.h>
#include <Core/Log/StdErrLogSink.h>
#include <Core/Performance/PerformanceAnalytics.h>
#include <Core/TimeFlow/Schedule/Scheduler.h>
#include <Core/TimeFlow/Timer.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Core/Type/TypeAlias.h>
#include <Core/Type/Variant/Variant.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Object/ObjectSystem.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/UI/UISystem.h>
#include <Platform/Input/GLFWInputAdapter.h>
#include <Platform/Window/Window.h>
#include <Platform/Window/WindowSystem.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/RenderPass/UIPass.h>
#include <Render/Rendering/RenderPipeline.h>
#include <Render/Rendering/Renderer.h>

#include <algorithm>
#include <cassert>
#include <stdexcept>

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

    const WindowDesc mainWindowDesc{
        .title = "URay Engine",
        .width = 1280,
        .height = 720,
        .resizable = true,
        .maximized = true
    };
    windowSystem = std::make_unique<WindowSystem>();
    if (!windowSystem->Initialize(mainWindowDesc))
    {
        URAY_LOG("[Engine] Failed to initialize window system.");
        Finalize();
        return false;
    }

    timer = std::make_unique<Timer>();
    scheduler = std::make_unique<Scheduler>();

    inputSystem = std::make_unique<InputSystem>();
    inputActionSystem = std::make_unique<InputActionSystem>();

    performanceAnalytics = std::make_unique<PerformanceAnalytics>();

    objectSystem = std::make_unique<ObjectSystem>();

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

    uiSystem = std::make_unique<UISystem>(windowSystem->GetMainWindow(), *inputSystem);
    uiSystem->Initialize();
    renderSystem->GetPipeline().AddRenderPass(std::make_unique<Render::UIPass>());

    assetSystem = std::make_unique<AssetSystem>();
    if (!assetSystem->Initialize(enginePath, projectPath))
    {
        URAY_LOG("[Engine] Failed to initialize asset system.");
        Finalize();
        return false;
    }

    windowSystem->GetMainWindow().Register("Key", inputSystem.get(), &InputSystem::OnKey);
    windowSystem->GetMainWindow().Register("MouseButton", inputSystem.get(), &InputSystem::OnMouseButton);
    windowSystem->GetMainWindow().Register("CursorMoved", inputSystem.get(), &InputSystem::OnCursorMoved);
    windowSystem->GetMainWindow().Register("TextInput", inputSystem.get(), &InputSystem::OnTextInput);
    windowSystem->GetMainWindow().Register("Scroll", inputSystem.get(), &InputSystem::OnScroll);
    windowSystem->GetMainWindow().Register("FocusChanged", inputSystem.get(), &InputSystem::OnWindowFocusChanged);

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

    Variant keyVariant(KeyCode::A);

    assert(keyVariant.GetType() == &TypeOf<KeyCode>());

    const KeyCode* key = keyVariant.TryGet<KeyCode>();

    assert(key != nullptr);
    assert(*key == KeyCode::A);

    const auto parameter = MakeFunctionParameter<KeyCode>();

    assert(parameter.has_value());
    assert(parameter->Accepts(keyVariant));

    return true;
}

void Engine::Finalize()
{
    if (renderSystem)
    {
        renderSystem->WaitIdle();
    }

    while (!additionalWindows.empty())
    {
        Window* window = additionalWindows.back();

        if (!DestroyAdditionalWindow(*window))
            throw std::runtime_error("Failed to destroy an additional window during engine finalization.");
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

    if (objectSystem)
    {
        objectSystem.reset();
    }

    if (performanceAnalytics)
    {
        performanceAnalytics.reset();
    }

    if (inputActionSystem)
    {
        inputActionSystem.reset();
    }

    if (windowSystem && inputSystem)
    {
        windowSystem->GetMainWindow().UnregisterAll(inputSystem.get());
    }

    if (inputSystem)
    {
        inputSystem.reset();
    }

    if (timer)
    {
        timer.reset();
    }

    if (windowSystem)
    {
        windowSystem->Finalize();
        windowSystem.reset();
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

bool Engine::BeginRender()
{
    URAY_PROFILE_SCOPE("Engine::BeginRender");

    renderSystem->GetPipeline().Reset();

    if (!renderSystem->BeginFrame())
        return false;

    uiSystem->PrepareRender();
    uiSystem->Paint();

    return true;
}

void Engine::Render(const Render::RenderRequest& request)
{
    URAY_PROFILE_SCOPE("Engine::Render");

    renderSystem->GetPipeline().Execute(request);
    renderSystem->EndFrame();
}

void Engine::RenderAdditionalWindows(
    const AdditionalWindowRenderRequestBuilder& buildRequest)
{
    for (Window* window : additionalWindows)
    {
        if (window->ShouldClose())
            continue;

        const Extent2D clientSize = window->GetClientSize();
        if (clientSize.width == 0 || clientSize.height == 0)
            continue;

        renderSystem->GetPipeline().Reset();

        if (!renderSystem->BeginFrame(*window))
            continue;

        Render::UIDrawContext* drawContext = uiSystem->PrepareAdditionalWindowUI(*window);

        if (!drawContext)
            throw std::runtime_error("Missing UI context for an active additional window.");

        Render::RenderRequest request = {};

        if (buildRequest)
            buildRequest(*window, request);

        request.uiDrawContext = drawContext;
        request.updateStats = false;

        Render(request);
    }
}

Window* Engine::CreateAdditionalWindow(const WindowDesc& desc)
{
    if (!windowSystem || !renderSystem || !inputSystem || !uiSystem)
        return nullptr;

    if (renderSystem->GetRenderer().IsFrameInProgress())
    {
        URAY_LOG("[Engine] Cannot create a window during a render frame.");
        return nullptr;
    }

    Window* window = windowSystem->CreateWindow(desc);
    if (!window)
        return nullptr;

    if (!renderSystem->CreateWindowSurface(*window))
    {
        windowSystem->DestroyWindow(*window);
        return nullptr;
    }

    WindowInputState* inputState = inputSystem->RegisterWindow(*window);

    if (!inputState)
    {
        URAY_LOG("[Engine] Failed to register window input state.");

        renderSystem->DestroyWindowSurface(*window);
        windowSystem->DestroyWindow(*window);
        return nullptr;
    }

    const bool inputRegistered =
        window->Register("Key", inputState, &WindowInputState::OnKey) &&
        window->Register("MouseButton", inputState, &WindowInputState::OnMouseButton) &&
        window->Register("CursorMoved", inputState, &WindowInputState::OnCursorMoved) &&
        window->Register("TextInput", inputState, &WindowInputState::OnTextInput) &&
        window->Register("Scroll", inputState, &WindowInputState::OnScroll) &&
        window->Register("FocusChanged", inputState, &WindowInputState::OnWindowFocusChanged);

    if (!inputRegistered)
    {
        URAY_LOG("[Engine] Failed to register window input handlers.");

        window->UnregisterAll(inputState);
        inputSystem->UnregisterWindow(*window);
        renderSystem->DestroyWindowSurface(*window);
        windowSystem->DestroyWindow(*window);
        return nullptr;
    }

    const bool focused = glfwGetWindowAttrib(window->GetGLFWWindow(), GLFW_FOCUSED) == GLFW_TRUE;

    double cursorX = 0.0;
    double cursorY = 0.0;
    glfwGetCursorPos(window->GetGLFWWindow(), &cursorX, &cursorY);

    inputState->OnWindowFocusChanged(focused);
    inputState->OnCursorMoved(Vector2(static_cast<float>(cursorX), static_cast<float>(cursorY)));
    inputState->PublishSnapshot(frameIndex);

    if (!uiSystem->RegisterWindow(*window))
    {
        URAY_LOG("[Engine] Failed to register window UI.");

        window->UnregisterAll(inputState);
        inputSystem->UnregisterWindow(*window);
        renderSystem->DestroyWindowSurface(*window);
        windowSystem->DestroyWindow(*window);
        return nullptr;
    }

    additionalWindows.push_back(window);
    return window;
}

bool Engine::DestroyAdditionalWindow(Window& window)
{
    if (!windowSystem || !renderSystem || !inputSystem || !uiSystem)
        return false;

    const auto it = std::find(additionalWindows.begin(), additionalWindows.end(), &window);

    if (it == additionalWindows.end())
        return false;

    if (renderSystem->GetRenderer().IsFrameInProgress())
    {
        URAY_LOG("[Engine] Cannot destroy a window during a render frame.");
        return false;
    }

    renderSystem->WaitIdle();

    if (!uiSystem->UnregisterWindow(window))
    {
        URAY_LOG("[Engine] Window UI context was not unregistered.");
        return false;
    }

    renderSystem->DestroyWindowSurface(window);

    if (WindowInputState* inputState = inputSystem->FindWindowState(window))
    {
        window.UnregisterAll(inputState);
        inputSystem->UnregisterWindow(window);
    }

    additionalWindows.erase(it);
    windowSystem->DestroyWindow(window);

    return true;
}

bool Engine::RequestCloseAdditionalWindow(Window& window)
{
    const auto it = std::find(additionalWindows.begin(), additionalWindows.end(), &window);
    if (it == additionalWindows.end())
        return false;

    window.RequestClose();
    return true;
}

void Engine::ProcessAdditionalWindowCloseRequests()
{
    size_t index = 0;

    while (index < additionalWindows.size())
    {
        Window* window = additionalWindows[index];
        if (!window->ShouldClose())
        {
            ++index;
            continue;
        }

        if (!DestroyAdditionalWindow(*window))
        {
            URAY_LOG("[Engine] Failed to process an additional window close request.");
            return;
        }
    }
}

} // namespace URay
