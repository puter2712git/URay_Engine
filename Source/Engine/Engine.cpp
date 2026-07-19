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
#include <iostream>

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

    editor = new Editor(*this, *renderer);
    if (!editor->Initialize())
        return false;

    timer = new Timer();

    meshManager = new MeshManager(*renderer);
    meshManager->CreateDefaultMeshes();

    Mesh* quadMesh = meshManager->GetMesh("quad");

    scene = new Scene();

    Unit* cameraUnit = new Unit();
    camera = new CameraComponent();
    camera->SetPosition(Vector3(0.0f, -5.0f, 0.0f));
    cameraUnit->AddComponent(camera);

    Unit* gridUnit = new Unit();
    MeshComponent* meshComponent = new MeshComponent();
    meshComponent->SetMesh(quadMesh);
    meshComponent->SetRotation(Vector3(-90.0f, 0.0f, 0.0f));
    meshComponent->SetScale(Vector3(10.0f, 10.0f, 10.0f));
    gridUnit->AddComponent(meshComponent);

    scene->AddUnit(cameraUnit);
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
        UpdatePick();

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

void Engine::SpawnUnit(Unit* unit)
{
    scene->AddUnit(unit);
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

void Engine::UpdatePick()
{
    if (inputManager.GetMouseDown(GLFW_MOUSE_BUTTON_LEFT))
    {
        float mouseX = inputManager.mouseX;
        float mouseY = inputManager.mouseY;

        float ndcX = (2.0f * mouseX) / 800.0f - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY) / 600.0f;

        Matrix invProjMatrix = camera->GetProjMatrix().Inverse();
        Matrix invViewMatrix = camera->GetViewMatrix().Inverse();

        const Vector4 ndcNear = Vector4(ndcX, ndcY, 0.0f, 1.0f);
        const Vector4 clipNear = ndcNear * invProjMatrix;
        const Vector4 viewNear = Vector4(clipNear.x / clipNear.w, clipNear.y / clipNear.w,
                                         clipNear.z / clipNear.w, 1.0f);
        const Vector4 worldNear4 = viewNear * invViewMatrix;
        const Vector3 start = Vector3(worldNear4.x, worldNear4.y, worldNear4.z);

        const Vector4 ndcFar = Vector4(ndcX, ndcY, 1.0f, 1.0f);
        const Vector4 clipFar = ndcFar * invProjMatrix;
        const Vector4 viewFar = Vector4(clipFar.x / clipFar.w, clipFar.y / clipFar.w,
                                        clipFar.z / clipFar.w, 1.0f);
        const Vector4 worldFar4 = viewFar * invViewMatrix;
        const Vector3 end = Vector3(worldFar4.x, worldFar4.y, worldFar4.z);

        const Vector3 lineDir = (end - start).GetNormalized();

        float minDist = std::numeric_limits<float>::max();
        bool isHit = false;

        for (const Unit* unit : scene->GetUnits())
        {
            std::set<Component*> components = unit->GetComponents();
            for (const Component* comp : components)
            {
                const MeshComponent* meshComponent = dynamic_cast<const MeshComponent*>(comp);
                if (!meshComponent)
                    continue;

                const Mesh* mesh = meshComponent->GetMesh();

                const std::vector<Vertex> vertices = mesh->GetVertices();
                const std::vector<uint16_t> indices = mesh->GetIndices();

                const Vector3 localStart = meshComponent->GetTransform().InvTransformPoint(start);
                const Vector3 localDir = meshComponent->GetTransform().InvTransformVectorNoScale(lineDir);

                for (size_t i = 0; i + 2 < indices.size(); i += 3)
                {
                    const Vector3 p0 = vertices[indices[i + 0]].pos;
                    const Vector3 p1 = vertices[indices[i + 1]].pos;
                    const Vector3 p2 = vertices[indices[i + 2]].pos;

                    const Vector3 e0 = p1 - p0;
                    const Vector3 e1 = p2 - p0;
                    const Vector3 norm = Vector3::Cross(e0, e1);

                    const float normDotDir = Vector3::Dot(norm, localDir);
                    if (std::fabs(normDotDir) < 0.001f)
                        continue;

                    float d = -Vector3::Dot(norm, p0);

                    float t = -(Vector3::Dot(norm, localStart) + d) / normDotDir;

                    if (t < 0)
                        continue;

                    Vector3 p = localStart + localDir * t;

                    Vector3 ne;

                    Vector3 p0p = p - p0;
                    ne = Vector3::Cross(e0, p0p);

                    if (Vector3::Dot(norm, ne) < 0)
                        continue;

                    Vector3 p2p1 = p2 - p1;
                    Vector3 p1p = p - p1;
                    ne = Vector3::Cross(p2p1, p1p);

                    if (Vector3::Dot(norm, ne) < 0)
                        continue;

                    Vector3 p2p0 = p0 - p2;
                    Vector3 p2p = p - p2;
                    ne = Vector3::Cross(p2p0, p2p);

                    if (Vector3::Dot(norm, ne) < 0)
                        continue;

                    if (minDist > t)
                    {
                        minDist = t;
                        isHit = true;
                    }
                }
            }
        }

        if (isHit)
        {
            std::cout << "Hit!" << std::endl;
        }
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
