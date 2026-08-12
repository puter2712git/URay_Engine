#include "Editor.h"

#include "Editor/Console/EditorConsole.h"
#include "Editor/EditorPicker.h"
#include "Editor/Filesystem/FilesystemWidget.h"
#include "Editor/MainMenuBar.h"
#include "Editor/PropertyDrawer.h"
#include "Editor/SceneTree.h"

#include "Engine/Component/ComponentFactory.h"
#include "Engine/Component/Render/GizmoComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Unit.h"

#include "Core/Log/Log.h"
#include "Core/Timer.h"

#include "Render/Renderer.h"

#include <imgui/imgui.h>

namespace URay
{

Editor::Editor(Engine& engine)
    : engine(engine)
{
}

bool Editor::Initialize()
{
    Renderer* renderer = engine.GetRenderer();

    if (!renderer->InitializeImGui())
        return false;

    picker = new EditorPicker(engine);
    mainMenuBar = new MainMenuBar();
    sceneTree = new SceneTree(*this, engine);
    console = new EditorConsole();
    filesystemWidget = new FilesystemWidget(*engine.GetFilesystem());

    Logger::Log("Hello, URay Engine!");

    return true;
}

void Editor::Finalize()
{
    delete console;
    delete sceneTree;

    Renderer* renderer = engine.GetRenderer();
    if (renderer)
    {
        renderer->FinalizeImGui();
    }
}

void Editor::Update()
{
    InputManager input = engine.GetInputManager();

    GizmoComponent* gizmo = engine.GetGizmo();

    if (input.GetMouseUp(GLFW_MOUSE_BUTTON_LEFT))
    {
        if (gizmo && gizmo->IsDragging())
        {
            gizmo->EndDragging();
        }
    }

    if (input.GetKeyDown(GLFW_KEY_SPACE))
    {
        if (gizmo)
        {
            GizmoMode mode = gizmo->GetCurrMode();
            int modeIndex = static_cast<int>(mode);
            modeIndex = (modeIndex + 1) % static_cast<int>(GizmoMode::Count);

            GizmoMode newMode = static_cast<GizmoMode>(modeIndex);

            gizmo->SetMode(newMode);
        }
    }

    UpdateHover();
    UpdatePick();
}

void Editor::PrepareRender()
{
    Renderer* renderer = engine.GetRenderer();

    renderer->BeginImGui();

    mainMenuBar->Draw();

    ShowStatus();
    ShowInspector();

    if (sceneTree)
    {
        sceneTree->Draw();
    }

    if (console)
    {
        bool open = true;
        console->Draw("Console", &open);
    }

    if (filesystemWidget)
    {
        filesystemWidget->Draw();
    }

    renderer->EndImGui();
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

void Editor::UpdateHover()
{
    InputManager input = engine.GetInputManager();

    GizmoComponent* gizmo = engine.GetGizmo();

    PickResult pickResult = picker->Pick(engine.GetCamera(), input.mouseX, input.mouseY);
    if (!pickResult.hit)
    {
        gizmo->SetHoveredAxis(-1);
        return;
    }

    if (pickResult.gizmoAxis != -1)
    {
        gizmo->SetHoveredAxis(pickResult.gizmoAxis);
    }
}

void Editor::UpdatePick()
{
    InputManager input = engine.GetInputManager();
    if (!input.GetMouseDown(GLFW_MOUSE_BUTTON_LEFT))
        return;

    GizmoComponent* gizmo = engine.GetGizmo();

    PickResult pickResult = picker->Pick(engine.GetCamera(), input.mouseX, input.mouseY);
    if (!pickResult.hit)
    {
        SelectUnit(nullptr);
        return;
    }

    if (pickResult.gizmoAxis != -1)
    {
        const Vector2 clickPos = Vector2(input.mouseX, input.mouseY);
        gizmo->StartDragging(clickPos, pickResult.gizmoAxis);
        return;
    }

    SelectUnit(pickResult.pickedUnit);
}

} // namespace URay
