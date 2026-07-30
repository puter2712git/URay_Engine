#include "Editor.h"

#include "Editor/MainMenuBar.h"
#include "Editor/PropertyDrawer.h"
#include "Editor/SceneTree.h"

#include "Engine/Component/ComponentFactory.h"
#include "Engine/Component/Render/GizmoComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene.h"
#include "Engine/Unit.h"

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

    mainMenuBar = new MainMenuBar();
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

    mainMenuBar->Draw();

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

    Class* cls = nullptr;
    std::vector<Property> properties;

    cls = selectedUnit->GetClass();
    properties = cls->GetProperties();

    for (Property& prop : properties)
    {
        ImGui::PushID(prop.name.c_str());

        PropertyDrawer::Draw(prop, selectedUnit);

        ImGui::PopID();
    }

    auto components = selectedUnit->GetComponents();
    for (Component* comp : components)
    {
        cls = comp->GetClass();
        properties = cls->GetProperties();

        ImGui::PushID(comp);

        ImGui::Text("%s", cls->GetName().c_str());

        for (Property& prop : properties)
        {
            ImGui::PushID(prop.name.c_str());

            PropertyDrawer::Draw(prop, comp);

            ImGui::PopID();
        }

        ImGui::PopID();
    }

    if (ImGui::BeginPopupContextWindow())
    {
        auto& components = ComponentFactory::GetRegisteredComponents();
        for (auto [name, constructor] : components)
        {
            std::string menuName = "Add " + name;
            if (ImGui::MenuItem(menuName.c_str()))
            {
                Component* newComp = constructor();
                selectedUnit->AddComponent(newComp);
            }
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

} // namespace URay
