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

bool Engine::Initialize()
{
    window = new Window();
    if (!window->Initialize())
        return false;

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
    CameraComponent* camera = new CameraComponent();
    camera->SetPosition(Vector3(0.0f, 3.0f, -5.0f));
    camera->SetRotation(Vector3(30.0f, 0, 0));
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
        glfwPollEvents();

        timer->Tick();

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

} // namespace URay
