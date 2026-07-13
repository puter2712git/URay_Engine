#include "Engine.h"

#include "Engine/Component/Component.h"
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

    Component* component = new Component();
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

        renderer->DrawFrame();
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
