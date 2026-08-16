#include "Engine.h"

#include "Engine/Asset/AssetPipeline.h"
#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/Render/GizmoComponent.h"
#include "Engine/Component/Render/GridComponent.h"
#include "Engine/Component/Render/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
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
    filesystem->Mount("Asset", fs::path(projectPath) / "Asset/Source");

    assetPipeline = new AssetPipeline(*filesystem);
    assetPipeline->Import("Asset://Texture/texture.jpg");

    shaderManager = renderer->GetShaderManager();

    materialManager = new MaterialManager(renderer->GetDevice(), renderer->GetResourceManager());
    materialManager->GetOrCreate("Mesh", shaderManager->GetOrCreate("Mesh"));
    materialManager->GetOrCreate("Sprite", shaderManager->GetOrCreate("Sprite"));

    meshManager = new MeshManager();
    meshManager->CreateDefaultMeshes();

    objImporter = new ObjImporter(*filesystem);
    objImporter->Import("Asset://Mesh/untitled.obj");

    textureManager = new TextureManager(*filesystem);
    textureManager->LoadTexture("Test", "Asset://Texture/texture.jpg");
    Texture* fontTexture = textureManager->LoadTexture("FontTexture", "Asset://Texture/DejaVu Sans Mono.png");

    fontManager = new FontManager();
    fontManager->LoadFont("Default", fontTexture);

    Mesh* quadMesh = meshManager->GetMesh("Quad");

    Scene* editorScene = new Scene(SceneType::Editor);

    Unit* cameraUnit = new Unit();
    cameraUnit->SetName("Editor Camera");
    TransformComponent* camTransform = new TransformComponent();
    camera = new CameraComponent();
    camTransform->SetPosition(Vector3(0.0f, -5.0f, 0.0f));
    cameraUnit->AddComponent(camTransform);
    cameraUnit->AddComponent(camera);

    Unit* gridUnit = new Unit();
    gridUnit->SetName("Grid");
    GridComponent* gridComponent = new GridComponent();
    gridUnit->AddComponent(gridComponent);

    Unit* gizmoUnit = new Unit();
    gizmoUnit->SetName("Gizmo");
    gizmo = new GizmoComponent();
    gizmo->SetEnabled(false);
    gizmoUnit->AddComponent(gizmo);

    editorScene->AddUnit(cameraUnit);
    editorScene->AddUnit(gridUnit);
    editorScene->AddUnit(gizmoUnit);

    scenes.push_back(editorScene);

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

    UpdateCameraMovement(timer->GetDeltaTime());
    UpdateCameraRotation(timer->GetDeltaTime());

    for (Scene* scene : scenes)
    {
        scene->Update(timer->GetDeltaTime());
    }
}

void Engine::PrepareRender()
{
    renderPipeline->Execute(scenes);
}

void Engine::Render()
{
    renderPipeline->EndFrame();
}

void Engine::SpawnUnit(Unit* unit)
{
    Scene* gameScene = GetSceneByType(SceneType::Game);
    if (gameScene)
    {
        gameScene->AddUnit(unit);
    }
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

void Engine::UpdateCameraMovement(float deltaTime)
{
    if (!camera)
        return;

    Vector3 moveDir = Vector3::Zero;

    if (inputManager.GetKey(GLFW_KEY_A))
    {
        moveDir.x -= 3.0f * deltaTime;
    }
    if (inputManager.GetKey(GLFW_KEY_D))
    {
        moveDir.x += 3.0f * deltaTime;
    }
    if (inputManager.GetKey(GLFW_KEY_W))
    {
        moveDir.y += 3.0f * deltaTime;
    }
    if (inputManager.GetKey(GLFW_KEY_S))
    {
        moveDir.y -= 3.0f * deltaTime;
    }

    const Unit* camUnit = camera->GetOwner();
    TransformComponent* transform = camUnit->GetTransform();

    Vector3 movePos = transform->TransformVectorNoScale(moveDir);
    Vector3 camPos = transform->GetPosition();

    if (inputManager.GetKey(GLFW_KEY_Q))
    {
        camPos.z -= 3.0f * deltaTime;
    }
    if (inputManager.GetKey(GLFW_KEY_E))
    {
        camPos.z += 3.0f * deltaTime;
    }

    transform->SetPosition(camPos + movePos);
}

void Engine::UpdateCameraRotation(float deltaTime)
{
    if (!camera)
        return;

    if (!inputManager.GetMouse(GLFW_MOUSE_BUTTON_RIGHT))
        return;

    const Unit* camUnit = camera->GetOwner();
    TransformComponent* transform = camUnit->GetTransform();

    Vector3 cameraRot = transform->GetRotation();
    cameraRot.x -= inputManager.mouseDeltaY * 0.1f;
    cameraRot.z -= inputManager.mouseDeltaX * 0.1f;

    cameraRot.x = std::clamp(cameraRot.x, -89.0f, 89.0f);

    transform->SetRotation(cameraRot);
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
