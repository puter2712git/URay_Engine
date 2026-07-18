#include "Engine.h"

#include "Core/Timer.h"
#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/Render/MeshComponent.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Scene.h"
#include "Engine/Unit.h"

#include "Platform/Window.h"

#include "Render/RenderPipeline.h"
#include "Render/Renderer.h"

#include "Editor/Editor.h"

#include <algorithm>

namespace URay
{

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

Engine* gEngine;

bool Engine::Initialize()
{
    gEngine = this;

    window = new Window();
    if (!window->Initialize())
        return false;

    glfwSetKeyCallback(window->GetGLFWWindow(), KeyCallback);
    glfwSetMouseButtonCallback(window->GetGLFWWindow(), MouseButtonCallback);
    glfwSetCursorPosCallback(window->GetGLFWWindow(), CursorPosCallback);

    renderer = new Renderer();
    if (!renderer->Initialize(window))
        return false;

    renderPipeline = new RenderPipeline(*renderer);

    editor = new Editor(*renderer);
    if (!editor->Initialize())
        return false;

    timer = new Timer();

    meshManager = new MeshManager(*renderer);

    // clang-format off
    std::vector<Vertex> boxVertices = {
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.2f, 0.2f } },
        { { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.6f, 0.2f } },
        { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 0.2f } },
        { { 0.5f, 0.5f, -0.5f }, { 0.2f, 1.0f, 0.2f } },
        { { -0.5f, -0.5f, 0.5f }, { 0.2f, 1.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.5f }, { 0.2f, 0.4f, 1.0f } },
        { { -0.5f, 0.5f, 0.5f }, { 0.6f, 0.2f, 1.0f } },
        { { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.2f, 1.0f } },
    };
    std::vector<uint16_t> boxIndices = {
        0, 2, 1, 1, 2, 3,
        4, 5, 6, 5, 7, 6,
        4, 6, 0, 0, 6, 2,
        1, 3, 5, 5, 3, 7,
        2, 6, 3, 3, 6, 7,
        4, 0, 5, 5, 0, 1,
    };
    std::vector<Vertex> quadVertices = {
        { { -0.5f, 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
        { { 0.5f, 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
        { { 0.5f, 0.0f, 0.5f }, { 1.0f, 1.0f, 1.0f } },
        { { -0.5f, 0.0f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
    };
    std::vector<uint16_t> quadIndices = {
        0, 1, 2,
        0, 2, 3,  
    };
    // clang-format on

    Mesh* boxMesh = meshManager->CreateMesh("box", boxVertices, boxIndices);
    Mesh* quadMesh = meshManager->CreateMesh("quad", quadVertices, quadIndices);

    scene = new Scene();

    Unit* cameraUnit = new Unit();
    camera = new CameraComponent();
    camera->SetPosition(Vector3(0.0f, -5.0f, 0.0f));
    cameraUnit->AddComponent(camera);

    Unit* boxUnit = new Unit();
    MeshComponent* component = new MeshComponent();
    component->SetMesh(boxMesh);
    boxUnit->AddComponent(component);

    Unit* gridUnit = new Unit();
    MeshComponent* meshComponent = new MeshComponent();
    meshComponent->SetMesh(quadMesh);
    meshComponent->SetRotation(Vector3(-90.0f, 0.0f, 0.0f));
    meshComponent->SetScale(Vector3(10.0f, 10.0f, 10.0f));
    gridUnit->AddComponent(meshComponent);

    scene->AddUnit(cameraUnit);
    scene->AddUnit(boxUnit);
    scene->AddUnit(gridUnit);

    return true;
}

void Engine::Run()
{
    while (!glfwWindowShouldClose(window->GetGLFWWindow()))
    {
        inputManager.Update();

        glfwPollEvents();

        timer->Tick();

        UpdateCameraMovement(timer->GetDeltaTime());
        UpdateCameraRotation(timer->GetDeltaTime());

        if (scene)
            scene->Update(timer->GetDeltaTime());

        renderPipeline->Execute(scene);
    }
}

void Engine::Finalize()
{
    editor->Finalize();
    delete editor;

    delete scene;

    delete meshManager;

    delete timer;

    renderer->Finalize();
    delete renderer;

    window->Finalize();
    delete window;
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

    Vector3 movePos = camera->GetTransform().TransformVectorNoScale(moveDir);
    Vector3 camPos = camera->GetPosition();

    if (inputManager.GetKey(GLFW_KEY_Q))
    {
        camPos.z -= 3.0f * deltaTime;
    }
    if (inputManager.GetKey(GLFW_KEY_E))
    {
        camPos.z += 3.0f * deltaTime;
    }

    camera->SetPosition(camPos + movePos);
}

void Engine::UpdateCameraRotation(float deltaTime)
{
    if (!camera)
        return;

    if (!inputManager.GetMouse(GLFW_MOUSE_BUTTON_RIGHT))
        return;

    Vector3 cameraRot = camera->GetRotation();
    cameraRot.x -= inputManager.mouseDeltaY * 0.1f;
    cameraRot.z -= inputManager.mouseDeltaX * 0.1f;

    cameraRot.x = std::clamp(cameraRot.x, -89.0f, 89.0f);

    camera->SetRotation(cameraRot);
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
