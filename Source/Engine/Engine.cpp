#include "Engine.h"

#include "Core/Timer.h"
#include "Engine/Component/Render/BoxComponent.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Scene.h"
#include "Engine/Unit.h"

#include "Platform/Window.h"

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

    Unit* unit = new Unit();

    BoxComponent* component = new BoxComponent();
    component->SetMesh(boxMesh);
    unit->AddComponent(component);

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

        renderer->Render(scene);
    }
}

void Engine::Finalize()
{
    delete scene;

    renderer->Finalize();
    delete renderer;

    window->Finalize();
    delete window;
}

} // namespace URay
