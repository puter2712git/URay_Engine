#include "Editor.h"

#include "Render/Renderer.h"

#include <imgui/imgui.h>

#include <iostream>

namespace URay
{

Editor::Editor(Renderer& renderer)
    : renderer(renderer)
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
        std::cout << "ImGui button clicked" << std::endl;

    ImGui::End();

    renderer.EndImGui();
}

} // namespace URay
