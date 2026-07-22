#include "Engine.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/Render/GizmoComponent.h"
#include "Engine/Component/Render/GridComponent.h"
#include "Engine/Component/Render/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Scene.h"
#include "Engine/Unit.h"

#include "Core/Math/Math.h"
#include "Core/Timer.h"

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
    TransformComponent* camTransform = new TransformComponent();
    camera = new CameraComponent();
    camTransform->SetPosition(Vector3(0.0f, -5.0f, 0.0f));
    cameraUnit->AddComponent(camTransform);
    cameraUnit->AddComponent(camera);

    Unit* gridUnit = new Unit();
    GridComponent* gridComponent = new GridComponent();
    gridUnit->AddComponent(gridComponent);

    Unit* gizmoUnit = new Unit();
    gizmo = new GizmoComponent();
    gizmo->SetEnabled(false);
    gizmoUnit->AddComponent(gizmo);

    scene->AddUnit(cameraUnit);
    scene->AddUnit(gridUnit);
    scene->AddUnit(gizmoUnit);

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
        UpdateHover();
        UpdatePick();

        if (scene)
            scene->Update(timer->GetDeltaTime());

        renderPipeline->Execute(scene);
    }
}

void Engine::Finalize()
{
    renderer->WaitIdle();

    editor->Finalize();
    delete editor;

    delete renderPipeline;
    renderPipeline = nullptr;

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

MaterialManager* Engine::GetMaterialManager() const
{
    return renderer->GetMaterialManager();
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

void Engine::UpdateHover()
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

    int axis;
    bool hovered = PickGizmo(start, lineDir, axis);

    if (hovered)
    {
        gizmo->SetHoveredAxis(axis);
    }
    else
    {
        gizmo->SetHoveredAxis(-1);
    }
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

        int gizmoAxis;
        bool gizmoHit = PickGizmo(start, lineDir, gizmoAxis);

        if (gizmoHit)
        {
            return;
        }

        float minDist = std::numeric_limits<float>::max();
        bool isHit = false;
        Unit* hitUnit = nullptr;

        for (Unit* unit : scene->GetUnits())
        {
            std::set<Component*> components = unit->GetComponents();
            for (const Component* comp : components)
            {
                const MeshComponent* meshComponent = dynamic_cast<const MeshComponent*>(comp);
                if (!meshComponent)
                    continue;

                TransformComponent* transform = unit->GetTransform();
                if (!transform)
                    continue;

                const Mesh* mesh = meshComponent->GetMesh();

                const std::vector<Vertex> vertices = mesh->GetVertices();
                const std::vector<uint16_t> indices = mesh->GetIndices();

                const Vector3 localStart = transform->InvTransformPoint(start);
                const Vector3 localDir = transform->InvTransformVectorNoScale(lineDir);

                for (size_t i = 0; i + 2 < indices.size(); i += 3)
                {
                    const Vector3 p0 = vertices[indices[i + 0]].pos;
                    const Vector3 p1 = vertices[indices[i + 1]].pos;
                    const Vector3 p2 = vertices[indices[i + 2]].pos;

                    float dist;
                    bool hit = Math::IntersectLineTriangle(
                        localStart, localDir,
                        p0, p1, p2,
                        dist);

                    if (!hit)
                        continue;

                    if (minDist > dist)
                    {
                        minDist = dist;
                        isHit = true;
                        hitUnit = unit;
                    }
                }
            }
        }

        if (isHit)
        {
            editor->SelectUnit(hitUnit);
        }
        else
        {
            editor->SelectUnit(nullptr);
        }
    }
}

bool Engine::PickGizmo(const Vector3& start, const Vector3& dir, int& outAxis)
{
    if (!gizmo || !gizmo->GetTarget())
        return false;

    bool gizmoHit = false;
    int hitAxis = -1;
    float minDist = std::numeric_limits<float>::max();

    for (size_t axis = 0; axis < 3; ++axis)
    {
        Mesh* gizmoMesh = gizmo->GetCurrMesh();
        const Matrix gizmoWorld = gizmo->GetCurrMatrix(axis);
        const Matrix invGizmoWorld = gizmoWorld.Inverse();

        const TransformComponent* targetTransform = gizmo->GetTargetTransform();

        const std::vector<Vertex> vertices = gizmoMesh->GetVertices();
        const std::vector<uint16_t> indices = gizmoMesh->GetIndices();

        const Vector3 localStart = invGizmoWorld.TransformPoint(start);
        const Vector3 localDir = invGizmoWorld.TransformVector(dir);

        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            const Vector3 p0 = vertices[indices[i + 0]].pos;
            const Vector3 p1 = vertices[indices[i + 1]].pos;
            const Vector3 p2 = vertices[indices[i + 2]].pos;

            float dist;
            bool hit = Math::IntersectLineTriangle(
                localStart, localDir,
                p0, p1, p2,
                dist);

            if (!hit)
                continue;

            if (dist < minDist)
            {
                minDist = dist;
                hitAxis = axis;
                gizmoHit = true;
            }
        }
    }

    if (gizmoHit)
    {
        outAxis = hitAxis;
        return true;
    }
    else
    {
        return false;
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
