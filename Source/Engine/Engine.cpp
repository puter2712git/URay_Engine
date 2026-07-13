#include "Engine.h"

#include "Engine/Component/Render/BoxComponent.h"
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

    scene = new Scene();

    Unit* unit = new Unit();

    BoxComponent* component = new BoxComponent();
    component->SetVertexBuffer(renderer->vertexBuffer);
    component->SetIndexBuffer(renderer->indexBuffer);
    unit->AddComponent(component);

    scene->AddUnit(unit);

    return true;
}

void Engine::Run()
{
    while (!glfwWindowShouldClose(window->GetGLFWWindow()))
    {
        glfwPollEvents();

        if (scene)
            scene->Update(0.16f);

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
