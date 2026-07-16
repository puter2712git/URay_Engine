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

    timer = new Timer();

    meshManager = new MeshManager(*renderer);

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
    // clang-format off
    std::vector<uint16_t> boxIndices = {
        0, 2, 1, 1, 2, 3,
        4, 5, 6, 5, 7, 6,
        4, 6, 0, 0, 6, 2,
        1, 3, 5, 5, 3, 7,
        2, 6, 3, 3, 6, 7,
        4, 0, 5, 5, 0, 1,
    };
    // clang-format on

    Mesh* boxMesh = meshManager->CreateMesh("box", boxVertices, boxIndices);

    scene = new Scene();

    Unit* cameraUnit = new Unit();
    camera = new CameraComponent();
    camera->SetPosition(Vector3(0.0f, -5.0f, 0.0f));
    cameraUnit->AddComponent(camera);

    Unit* unit = new Unit();
    MeshComponent* component = new MeshComponent();
    component->SetMesh(boxMesh);
    unit->AddComponent(component);

    scene->AddUnit(cameraUnit);
    scene->AddUnit(unit);

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

        if (scene)
            scene->Update(timer->GetDeltaTime());

        renderPipeline->Execute(scene);
    }
}

void Engine::Finalize()
{
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

    Vector3 cameraPos = camera->GetPosition();
    if (inputManager.GetKey(GLFW_KEY_A))
    {
        cameraPos.x -= 3.0f * deltaTime;
        camera->SetPosition(cameraPos);
    }
    if (inputManager.GetKey(GLFW_KEY_D))
    {
        cameraPos.x += 3.0f * deltaTime;
        camera->SetPosition(cameraPos);
    }
    if (inputManager.GetKey(GLFW_KEY_W))
    {
        cameraPos.y += 3.0f * deltaTime;
        camera->SetPosition(cameraPos);
    }
    if (inputManager.GetKey(GLFW_KEY_S))
    {
        cameraPos.y -= 3.0f * deltaTime;
        camera->SetPosition(cameraPos);
    }
    if (inputManager.GetKey(GLFW_KEY_Q))
    {
        cameraPos.z -= 3.0f * deltaTime;
        camera->SetPosition(cameraPos);
    }
    if (inputManager.GetKey(GLFW_KEY_E))
    {
        cameraPos.z += 3.0f * deltaTime;
        camera->SetPosition(cameraPos);
    }
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
