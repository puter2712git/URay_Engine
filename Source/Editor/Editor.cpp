#include "Editor.h"

#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/TestUnit.h"

#include "Core/Timer.h"

#include "Render/Renderer.h"

#include <imgui/imgui.h>

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

    ShowTestPanel();
    ShowStatus();
    ShowInspector();

    renderer.EndImGui();
}

void Editor::SelectUnit(Unit* unit)
{
    selectedUnit = unit;
}

void Editor::ShowTestPanel() const
{
    ImGui::Begin("Hello, world!");

    if (ImGui::Button("Click me"))
    {
        TestUnit* testUnit = new TestUnit();
        engine.GetScene()->AddUnit(testUnit);
    }

    ImGui::End();
}

void Editor::ShowStatus() const
{
    ImGui::Begin("Status");

    ImGui::Text("FPS: %d", gEngine->GetTimer()->GetFPS());
    ImGui::Text("%.4f ms", gEngine->GetTimer()->GetDeltaTime());

    ImGui::End();
}

void Editor::ShowInspector() const
{
    ImGui::Begin("Inspector");

    if (!selectedUnit)
    {
        ImGui::End();
        return;
    }

    Transform newTransform = selectedUnit->GetTransform();
    if (ImGui::DragFloat3("Position", &newTransform.position.x))
    {
        selectedUnit->SetPosition(newTransform.position);
    }
    if (ImGui::DragFloat3("Rotation", &newTransform.rotation.x))
    {
        selectedUnit->SetRotation(newTransform.rotation);
    }
    if (ImGui::DragFloat3("Scale", &newTransform.scale.x))
    {
        selectedUnit->SetScale(newTransform.scale);
    }

    ImGui::End();
}

} // namespace URay
