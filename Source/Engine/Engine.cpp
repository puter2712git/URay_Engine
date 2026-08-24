#include "Engine.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Font/FontManager.h"
#include "Engine/Importer/ObjImporter.h"
#include "Engine/Material/Material.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Spatial/Octree.h"
#include "Engine/Texture/TextureManager.h"
#include "Engine/Unit.h"

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

    filesystem = new VirtualFilesystem();
    filesystem->Mount("Project", projectPath);
    filesystem->Mount("RawAsset", fs::path(projectPath) / "Asset/Source");
    filesystem->Mount("Asset", fs::path(projectPath) / "Asset/Imported");

    renderer = new RHI::Renderer();
    if (!renderer->Initialize(window, *filesystem))
        return false;

    renderPipeline = new RHI::RenderPipeline(*renderer);

    performanceAnalytics = std::make_unique<PerformanceAnalytics>();

    shaderManager = renderer->GetShaderManager();

    textureManager = new TextureManager(*filesystem);
    textureManager->LoadTexture("Test", "RawAsset://Texture/texture.jpg");
    Texture* fontTexture = textureManager->LoadTexture("FontTexture", "RawAsset://Texture/DejaVu Sans Mono.png");
    Texture* defaultWhite = textureManager->LoadTexture("DefaultWhite", "RawAsset://Texture/white.png");

    materialManager = new MaterialManager(renderer->GetDevice(), renderer->GetResourceManager(), defaultWhite);
    Material* defaultMaterial = materialManager->GetOrCreate("Mesh", shaderManager->GetOrCreate("Mesh"));
    materialManager->GetOrCreate("Sprite", shaderManager->GetOrCreate("Sprite"));

    meshManager = new MeshManager();
    meshManager->CreateDefaultMeshes(defaultMaterial);

    objImporter = new ObjImporter(*filesystem, *meshManager, *textureManager, *materialManager, defaultMaterial->GetShader());
    objImporter->Import("RawAsset://Mesh/untitled.obj");
    objImporter->Import("RawAsset://Mesh/SilverWolf/SilverWolf.obj");

    Mesh* appleMesh = objImporter->Import("RawAsset://Mesh/apple_mid.obj");
    Mesh* bittenAppleMesh = objImporter->Import("RawAsset://Mesh/bitten_apple_mid.obj");

    fontManager = new FontManager();
    fontManager->LoadFont("Default", fontTexture);

    Mesh* quadMesh = meshManager->GetMesh("Quad");

    Scene* gameScene = new Scene(SceneType::Game);
    renderer->CreateRenderScene(gameScene);

    constexpr uint32_t gridWidth = 50;
    constexpr uint32_t gridDepth = 40;
    constexpr uint32_t gridHeight = 2;
    constexpr float spacing = 1.5f;

    for (uint32_t z = 0; z < gridHeight; ++z)
    {
        for (uint32_t y = 0; y < gridDepth; ++y)
        {
            for (uint32_t x = 0; x < gridWidth; ++x)
            {
                Unit* appleUnit = new Unit();
                appleUnit->SetName("Apple");

                TransformComponent* transform = new TransformComponent();
                transform->SetPosition(Vector3(
                    (static_cast<float>(x) - (gridWidth - 1) * 0.5f) * spacing,
                    (static_cast<float>(y) - (gridDepth - 1) * 0.5f) * spacing,
                    (static_cast<float>(z) - (gridHeight - 1) * 0.5f) * spacing));

                MeshComponent* meshComponent = new MeshComponent();
                meshComponent->SetMesh((x + y + z) % 2 == 0 ? appleMesh : bittenAppleMesh);

                appleUnit->AddComponent(transform);
                appleUnit->AddComponent(meshComponent);
                gameScene->AddUnit(appleUnit);
            }
        }
    }

    scenes.push_back(gameScene);

    return true;
}

void Engine::Finalize()
{
    renderer->WaitIdle();

    delete renderPipeline;
    renderPipeline = nullptr;

    for (Scene* scene : scenes)
    {
        delete scene;
    }

    delete materialManager;

    delete meshManager;

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

    std::vector<RHI::RenderScene*> renderScenes;
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

RHI::GPUResourceManager* Engine::GetGPUResourceManager() const
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
