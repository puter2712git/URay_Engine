#include "Engine.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Component/Render/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Asset/Font/FontManager.h"
#include "Engine/Asset/Importer/ObjImporter.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Material/MaterialManager.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Mesh/MeshManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Spatial/Octree.h"
#include "Engine/Asset/Texture/TextureManager.h"
#include "Engine/Scene/Unit.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Math/Frustum.h"
#include "Core/Performance/PerformanceAnalytics.h"
#include "Core/Timer.h"

#include "Platform/Input/GLFWInputAdapter.h"
#include "Platform/Window/Window.h"

#include "Render/RenderPipeline.h"
#include "Render/Renderer.h"
#include "Render/Shader/ShaderManager.h"

#include "Editor/Editor.h"

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

bool Engine::Initialize(const std::string& projectPath)
{
    gEngine = this;

    window = new Window();
    if (!window->Initialize())
        return false;

    glfwSetKeyCallback(window->GetGLFWWindow(), KeyCallback);
    glfwSetMouseButtonCallback(window->GetGLFWWindow(), MouseButtonCallback);
    glfwSetCursorPosCallback(window->GetGLFWWindow(), CursorPosCallback);

    timer = new Timer();

    assetSystem = std::make_unique<AssetSystem>();
    if (!assetSystem->Initialize(projectPath))
        return false;

    renderer = new Render::Renderer();
    if (!renderer->Initialize(window, assetSystem->GetFilesystem()))
        return false;

    renderPipeline = new Render::RenderPipeline(*renderer);

    performanceAnalytics = std::make_unique<PerformanceAnalytics>();

    if (!assetSystem->InitializeRuntimeAssets(
            *renderer->GetDevice(),
            *renderer->GetResourceManager(),
            *renderer->GetShaderManager()))
    {
        return false;
    }

    Scene* gameScene = new Scene(SceneType::Game);
    renderer->CreateRenderScene(gameScene);

    scenes.push_back(gameScene);

    return true;
}

void Engine::Finalize()
{
    renderer->WaitIdle();

    if (assetSystem)
    {
        assetSystem.reset();
    }

    delete renderPipeline;
    renderPipeline = nullptr;

    for (Scene* scene : scenes)
    {
        delete scene;
    }

    delete timer;

    renderer->Finalize();
    delete renderer;

    window->Finalize();
    delete window;
}

void Engine::Update()
{
    URAY_PROFILE_SCOPE("Engine::Update");

    inputManager.ClearEvents();
    inputManager.Update();

    glfwPollEvents();

    timer->Tick();

    for (Scene* scene : scenes)
    {
        scene->Update(timer->GetDeltaTime());
    }
}

void Engine::BeginRender()
{
    URAY_PROFILE_SCOPE("Engine::BeginRender");

    renderPipeline->Reset();
    renderer->BeginFrame();
}

void Engine::PrepareRender()
{
    URAY_PROFILE_SCOPE("Engine::PrepareRender");
}

void Engine::Render()
{
    URAY_PROFILE_SCOPE("Engine::Render");

    const auto& scenes = renderer->GetScenes();

    std::vector<Render::RenderScene*> renderScenes;
    for (const auto& [scene, renderScene] : scenes)
    {
        renderScenes.push_back(renderScene);
    }

    renderer->BeginScenePass();
    renderPipeline->Execute(renderScenes);
    renderer->EndScenePass();
}

void Engine::EndRender()
{
    URAY_PROFILE_SCOPE("Engine::EndRender");

    renderer->BeginSwapChainPass();
    renderer->EndImGui();
    renderer->EndSwapChainPass();
    renderer->EndFrame();
}

void Engine::SpawnUnit(Unit* unit)
{
    Scene* gameScene = GetSceneByType(SceneType::Game);
    if (gameScene)
    {
        gameScene->AddUnit(unit);
    }
}

void Engine::AddScene(Scene* scene)
{
    scenes.push_back(scene);
}

void Engine::SetGameScene(Scene* gameScene)
{
    if (!gameScene)
        return;

    Scene* currGameScene = GetSceneByType(SceneType::Game);
    if (currGameScene)
    {
        scenes.erase(std::remove_if(scenes.begin(), scenes.end(),
                                    [&](Scene* scene)
                                    {
                                        return currGameScene == scene;
                                    }),
                     scenes.end());

        delete currGameScene;
        currGameScene = nullptr;
    }

    scenes.push_back(gameScene);
}

void Engine::GetWindowSize(int& width, int& height) const
{
    glfwGetWindowSize(window->GetGLFWWindow(), &width, &height);
}

void Engine::GetFramebufferSize(int& width, int& height) const
{
    VkExtent2D extent = renderer->GetSwapChainExtent();
    width = static_cast<int>(extent.width);
    height = static_cast<int>(extent.height);
}

Render::GPUResourceManager* Engine::GetGPUResourceManager() const
{
    return renderer->GetResourceManager();
}

Scene* Engine::GetSceneByType(SceneType type) const
{
    for (Scene* scene : scenes)
    {
        if (scene->GetType() == type)
        {
            return scene;
        }
    }

    return nullptr;
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
