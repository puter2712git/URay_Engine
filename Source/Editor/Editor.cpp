#include "Editor.h"

#include "Editor/Console/EditorConsole.h"
#include "Editor/EditorPicker.h"
#include "Editor/Filesystem/FilesystemWidget.h"
#include "Editor/GizmoController.h"
#include "Editor/MainMenuBar.h"
#include "Editor/PropertyDrawer.h"
#include "Editor/SceneTree.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/ComponentFactory.h"
#include "Engine/Component/Render/GridComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Unit.h"

#include "Core/Log/Log.h"
#include "Core/Timer.h"

#include "Render/RenderPipeline.h"
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
    RHI::Renderer* renderer = engine.GetRenderer();

    if (!renderer->InitializeImGui())
        return false;

    picker = new EditorPicker(engine, gizmo);
    mainMenuBar = new MainMenuBar();
    sceneTree = new SceneTree(*this, engine);
    console = new EditorConsole();
    filesystemWidget = new FilesystemWidget(*engine.GetFilesystem());
    gizmo = new GizmoController(*engine.GetMeshManager(), *engine.GetMaterialManager());

    PrepareEditorScene();

    return true;
}

void Editor::Finalize()
{
    delete console;
    delete sceneTree;

    RHI::Renderer* renderer = engine.GetRenderer();
    if (renderer)
    {
        renderer->FinalizeImGui();
    }
}

void Editor::Update()
{
    InputManager input = engine.GetInputManager();

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
            GizmoMode mode = gizmo->GetMode();
            int modeIndex = static_cast<int>(mode);
            modeIndex = (modeIndex + 1) % static_cast<int>(GizmoMode::Count);

            GizmoMode newMode = static_cast<GizmoMode>(modeIndex);

            gizmo->SetMode(newMode);
        }
    }

    UpdateHover();
    UpdatePick();

    Timer* timer = engine.GetTimer();
    float deltaTime = timer->GetDeltaTime();

    UpdateCameraMovement(deltaTime);
    UpdateCameraRotation(deltaTime);
}

void Editor::PrepareRender()
{
    RHI::Renderer* renderer = engine.GetRenderer();
    RHI::DrawCommandBuilder& builder = engine.GetRenderPipeline()->GetBuilder();

    gizmo->Draw(builder);

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

    gizmo->SetTarget(unit);
}

void Editor::PrepareEditorScene()
{
    Scene* editorScene = new Scene(SceneType::Editor);

    Unit* cameraUnit = new Unit();
    cameraUnit->SetName("Editor Camera");

    TransformComponent* cameraTransform = new TransformComponent();
    cameraTransform->SetPosition(Vector3(0.0f, -5.0f, 0.0f));
    camera = new CameraComponent();
    cameraUnit->AddComponent(cameraTransform);
    cameraUnit->AddComponent(camera);

    Unit* gridUnit = new Unit();
    gridUnit->SetName("Grid");
    GridComponent* gridComponent = new GridComponent();
    gridUnit->AddComponent(gridComponent);

    editorScene->AddUnit(cameraUnit);
    editorScene->AddUnit(gridUnit);

    engine.AddScene(editorScene);
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

void Editor::UpdateCameraMovement(float deltaTime)
{
    if (!camera)
        return;

    Vector3 moveDir = Vector3::Zero;

    InputManager& input = engine.GetInputManager();

    if (input.GetKey(GLFW_KEY_A))
    {
        moveDir.x -= 3.0f * deltaTime;
    }
    if (input.GetKey(GLFW_KEY_D))
    {
        moveDir.x += 3.0f * deltaTime;
    }
    if (input.GetKey(GLFW_KEY_W))
    {
        moveDir.y += 3.0f * deltaTime;
    }
    if (input.GetKey(GLFW_KEY_S))
    {
        moveDir.y -= 3.0f * deltaTime;
    }

    const Unit* camUnit = camera->GetOwner();
    TransformComponent* transform = camUnit->GetTransform();

    Vector3 movePos = transform->TransformVectorNoScale(moveDir);
    Vector3 camPos = transform->GetPosition();

    if (input.GetKey(GLFW_KEY_Q))
    {
        camPos.z -= 3.0f * deltaTime;
    }
    if (input.GetKey(GLFW_KEY_E))
    {
        camPos.z += 3.0f * deltaTime;
    }

    transform->SetPosition(camPos + movePos);
}

void Editor::UpdateCameraRotation(float deltaTime)
{
    if (!camera)
        return;

    InputManager& input = engine.GetInputManager();

    if (!input.GetMouse(GLFW_MOUSE_BUTTON_RIGHT))
        return;

    const Unit* camUnit = camera->GetOwner();
    TransformComponent* transform = camUnit->GetTransform();

    Vector3 cameraRot = transform->GetRotation();
    cameraRot.x -= input.mouseDeltaY * 0.1f;
    cameraRot.z -= input.mouseDeltaX * 0.1f;

    cameraRot.x = std::clamp(cameraRot.x, -89.0f, 89.0f);

    transform->SetRotation(cameraRot);
}

void Editor::UpdateHover()
{
    InputManager input = engine.GetInputManager();

    PickResult pickResult = picker->Pick(camera, input.mouseX, input.mouseY);
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

    PickResult pickResult = picker->Pick(camera, input.mouseX, input.mouseY);
    if (!pickResult.hit)
    {
        SelectUnit(nullptr);
        return;
    }

    if (pickResult.gizmoAxis != -1)
    {
        const Vector2 clickPos = Vector2(input.mouseX, input.mouseY);
        gizmo->StartDragging(clickPos, pickResult.gizmoAxis, camera);
        return;
    }

    SelectUnit(pickResult.pickedUnit);
}

} // namespace URay
