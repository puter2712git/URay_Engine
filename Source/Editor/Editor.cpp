#include "Editor.h"

#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/TestUnit.h"

#include "Render/Renderer.h"

#include <imgui/imgui.h>

#include <iostream>

namespace URay
{

Editor::Editor(Engine& engine, Renderer& renderer)
    : engine(engine), renderer(renderer)
{
}

bool Editor::Initialize()
{
    if (!renderer.InitializeImGui())
        return false;

    return true;
}

void Editor::Finalize()
{
    renderer.FinalizeImGui();
}

void Editor::Update(float deltaTime)
{
}

void Editor::Render()
{
    renderer.BeginImGui();

    ImGui::Begin("Hello, world!");

    if (ImGui::Button("Click me"))
    {
        TestUnit* testUnit = new TestUnit();
        engine.GetScene()->AddUnit(testUnit);
    }

    ImGui::End();

    renderer.EndImGui();
}

} // namespace URay
