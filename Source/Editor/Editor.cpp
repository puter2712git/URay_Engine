#include "Editor.h"

#include "Editor/PropertyDrawer.h"
#include "Editor/SceneTree.h"

#include "Engine/Component/Render/GizmoComponent.h"
#include "Engine/Component/TransformComponent.h"
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

    sceneTree = new SceneTree();

    return true;
}

void Editor::Finalize()
{
    delete sceneTree;

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
    
    if (sceneTree)
        sceneTree->Draw();

    renderer.EndImGui();
}

void Editor::SelectUnit(Unit* unit)
{
    selectedUnit = unit;

    engine.GetGizmo()->SetTarget(unit);
}

void Editor::ShowTestPanel() const
{
    ImGui::Begin("Hello, world!");

    if (ImGui::Button("Click me"))
    {
        TestUnit* testUnit = new TestUnit();
        testUnit->SetName("Test Unit");
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

    auto components = selectedUnit->GetComponents();
    for (Component* comp : components)
    {
        std::vector<Property> props;
        comp->GetProperties(props);

        ImGui::PushID(comp);

        ImGui::Text("%s", comp->GetName().c_str());

        for (Property& prop : props)
        {
            ImGui::PushID(prop.name.c_str());

            PropertyDrawer::Draw(prop, comp);

            ImGui::PopID();
        }

        ImGui::PopID();
    }

    ImGui::End();
}

} // namespace URay
