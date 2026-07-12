#include "Engine.h"

#include "Engine/Component/Component.h"
#include "Engine/Unit.h"
#include "Platform/Window.h"
#include "Render/Renderer.h"

namespace URay
{

Engine::~Engine()
{
}

bool Engine::Initialize()
{
    window = new Window();
    if (!window->Initialize())
        return false;

    renderer = new Renderer();
    if (!renderer->Initialize(window))
        return false;

    unit = new Unit();

    Component* component = new Component();
    unit->AddComponent(component);

    return true;
}

void Engine::Run()
{
    while (!glfwWindowShouldClose(window->GetGLFWWindow()))
    {
        glfwPollEvents();

        if (unit)
            unit->Update(0.16f);

        renderer->DrawFrame();
    }
}

void Engine::Finalize()
{
    delete unit;

    renderer->Finalize();
    delete renderer;

    window->Finalize();
    delete window;
}

} // namespace URay
