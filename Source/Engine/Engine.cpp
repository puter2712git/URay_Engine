#include "Engine.h"

#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Font/FontManager.h"
#include "Engine/Importer/ObjImporter.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Texture/TextureManager.h"
#include "Engine/Unit.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Timer.h"

#include "Platform/Window.h"

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

bool Engine::Initialize(const std::string& projectPath)
{
    gEngine = this;

    window = new Window();
    if (!window->Initialize())
        return false;

    glfwSetKeyCallback(window->GetGLFWWindow(), KeyCallback);
    glfwSetMouseButtonCallback(window->GetGLFWWindow(), MouseButtonCallback);
    glfwSetCursorPosCallback(window->GetGLFWWindow(), CursorPosCallback);

    renderer = new RHI::Renderer();
    if (!renderer->Initialize(window))
        return false;

    renderPipeline = new RHI::RenderPipeline(*renderer);

    timer = new Timer();

    filesystem = new VirtualFilesystem();
    filesystem->Mount("Project", projectPath);
    filesystem->Mount("RawAsset", fs::path(projectPath) / "Asset/Source");
    filesystem->Mount("Asset", fs::path(projectPath) / "Asset/Imported");

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

    objImporter = new ObjImporter(*filesystem);
    objImporter->Import("RawAsset://Mesh/untitled.obj");

    fontManager = new FontManager();
    fontManager->LoadFont("Default", fontTexture);

    Mesh* quadMesh = meshManager->GetMesh("Quad");

    Scene* gameScene = new Scene(SceneType::Game);
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
    renderPipeline->Reset();
    renderer->BeginFrame();
}

void Engine::PrepareRender()
{
    for (const Scene* scene : scenes)
    {
        for (const Unit* unit : scene->GetUnits())
        {
            if (RenderComponent* comp = unit->GetComponent<RenderComponent>())
            {
                comp->SubmitCommand(renderPipeline->GetBuilder());
            }
        }
    }
}

void Engine::Render()
{
    renderPipeline->Execute(scenes);
}

void Engine::EndRender()
{
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
    if (key < 0)
        return;

    gEngine->GetInputManager().currKeys[key] = action != GLFW_RELEASE;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    gEngine->GetInputManager().currMouse[button] = action != GLFW_RELEASE;
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    gEngine->GetInputManager().mouseX = xpos;
    gEngine->GetInputManager().mouseY = ypos;
}

} // namespace URay
