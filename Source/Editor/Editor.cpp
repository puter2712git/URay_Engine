#include "Editor.h"

#include "Editor/EditorPicker.h"
#include "Editor/GizmoController.h"
#include "Editor/Widget/Console/ConsoleWidget.h"
#include "Editor/Widget/Filesystem/FilesystemWidget.h"
#include "Editor/Widget/InspectorWidget.h"
#include "Editor/Widget/MainMenuBarWidget.h"
#include "Editor/Widget/SceneTreeWidget.h"
#include "Editor/Widget/Splitter.h"
#include "Editor/Widget/StatusWidget.h"
#include "Editor/Widget/ViewportWidget.h"
#include "Editor/Widget/Widget.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/Render/GridComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Unit.h"

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

Editor::~Editor() = default;

bool Editor::Initialize()
{
    RHI::Renderer* renderer = engine.GetRenderer();

    if (!renderer->InitializeImGui())
        return false;

    CameraComponent& camera = PrepareEditorScene();

    mainMenuBarWidget = std::make_unique<MainMenuBarWidget>(engine);

    std::unique_ptr<SceneTreeWidget> sceneTree = std::make_unique<SceneTreeWidget>(*this, engine);
    std::unique_ptr<InspectorWidget> inspector = std::make_unique<InspectorWidget>(*this);
    std::unique_ptr<ConsoleWidget> console = std::make_unique<ConsoleWidget>();
    std::unique_ptr<FilesystemWidget> filesystem = std::make_unique<FilesystemWidget>(*engine.GetFilesystem());
    std::unique_ptr<StatusWidget> status = std::make_unique<StatusWidget>(engine);
    std::unique_ptr<ViewportWidget> viewport = std::make_unique<ViewportWidget>(*renderer, camera, engine, [this](Unit* unit)
                                                                                { SelectUnit(unit); });
    viewportWidget = viewport.get();

    std::unique_ptr<Splitter> rightPanel2 = std::make_unique<Splitter>(SplitAxis::Vertical, std::move(sceneTree), std::move(inspector));
    std::unique_ptr<Splitter> rightPanel = std::make_unique<Splitter>(SplitAxis::Vertical, std::move(status), std::move(rightPanel2));
    std::unique_ptr<Splitter> leftPanel2 = std::make_unique<Splitter>(SplitAxis::Horizontal, std::move(console), std::move(filesystem));
    std::unique_ptr<Splitter> leftPanel = std::make_unique<Splitter>(SplitAxis::Vertical, std::move(viewport), std::move(leftPanel2));

    rootWidget = std::make_unique<Splitter>(
        SplitAxis::Horizontal,
        std::move(leftPanel),
        std::move(rightPanel));

    return true;
}

void Editor::Finalize()
{
    mainMenuBarWidget.reset();
    rootWidget.reset();

    RHI::Renderer* renderer = engine.GetRenderer();
    if (renderer)
    {
        renderer->FinalizeImGui();
    }
}

void Editor::Update()
{
    // UpdateHover();
    // UpdatePick();

    Timer* timer = engine.GetTimer();
    float deltaTime = timer->GetDeltaTime();

    // UpdateCameraMovement(deltaTime);
    // UpdateCameraRotation(deltaTime);

    // gizmo->Update(camera);

    inputRouter.Process(*rootWidget, engine.GetInputManager());

    mainMenuBarWidget->Update();
    rootWidget->Update();
}

void Editor::PrepareRender()
{
    RHI::Renderer* renderer = engine.GetRenderer();
    RHI::DrawCommandBuilder& builder = engine.GetRenderPipeline()->GetBuilder();

    rootWidget->PrepareRender(builder);

    renderer->BeginImGui();

    mainMenuBarWidget->Draw();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    rootWidget->Arrange({
        Vector2(viewport->WorkPos.x, viewport->WorkPos.y),
        Vector2(viewport->WorkSize.x, viewport->WorkSize.y),
    });

    rootWidget->Draw();
}

void Editor::SelectUnit(Unit* unit)
{
    selectedUnit = unit;

    if (viewportWidget)
    {
        viewportWidget->SetSelectedUnit(unit);
    }
}

CameraComponent& Editor::PrepareEditorScene()
{
    Scene* editorScene = new Scene(SceneType::Editor);

    Unit* cameraUnit = new Unit();
    cameraUnit->SetName("Editor Camera");

    TransformComponent* cameraTransform = new TransformComponent();
    cameraTransform->SetPosition(Vector3(0.0f, -5.0f, 0.0f));
    CameraComponent* camera = new CameraComponent();
    cameraUnit->AddComponent(cameraTransform);
    cameraUnit->AddComponent(camera);

    Unit* gridUnit = new Unit();
    gridUnit->SetName("Grid");
    GridComponent* gridComponent = new GridComponent();
    gridUnit->AddComponent(gridComponent);

    editorScene->AddUnit(cameraUnit);
    editorScene->AddUnit(gridUnit);

    engine.AddScene(editorScene);

    return *camera;
}

// void Editor::UpdateCameraMovement(float deltaTime)
//{
//     if (!camera)
//         return;
//
//     Vector3 moveDir = Vector3::Zero;
//
//     InputManager& input = engine.GetInputManager();
//
//     if (input.GetKey(GLFW_KEY_A))
//     {
//         moveDir.x -= 3.0f * deltaTime;
//     }
//     if (input.GetKey(GLFW_KEY_D))
//     {
//         moveDir.x += 3.0f * deltaTime;
//     }
//     if (input.GetKey(GLFW_KEY_W))
//     {
//         moveDir.y += 3.0f * deltaTime;
//     }
//     if (input.GetKey(GLFW_KEY_S))
//     {
//         moveDir.y -= 3.0f * deltaTime;
//     }
//
//     const Unit* camUnit = camera->GetOwner();
//     TransformComponent* transform = camUnit->GetTransform();
//
//     Vector3 movePos = transform->TransformVectorNoScale(moveDir);
//     Vector3 camPos = transform->GetPosition();
//
//     if (input.GetKey(GLFW_KEY_Q))
//     {
//         camPos.z -= 3.0f * deltaTime;
//     }
//     if (input.GetKey(GLFW_KEY_E))
//     {
//         camPos.z += 3.0f * deltaTime;
//     }
//
//     transform->SetPosition(camPos + movePos);
// }

// void Editor::UpdateCameraRotation(float deltaTime)
//{
//     if (!camera)
//         return;
//
//     InputManager& input = engine.GetInputManager();
//
//     if (!input.GetMouse(GLFW_MOUSE_BUTTON_RIGHT))
//         return;
//
//     const Unit* camUnit = camera->GetOwner();
//     TransformComponent* transform = camUnit->GetTransform();
//
//     Vector3 cameraRot = transform->GetRotation();
//     cameraRot.x -= input.mouseDeltaY * 0.1f;
//     cameraRot.z -= input.mouseDeltaX * 0.1f;
//
//     cameraRot.x = std::clamp(cameraRot.x, -89.0f, 89.0f);
//
//     transform->SetRotation(cameraRot);
// }

// void Editor::UpdateHover()
//{
//     InputManager input = engine.GetInputManager();
//
//     PickResult pickResult = picker->Pick(camera, input.mouseX, input.mouseY);
//     if (!pickResult.hit)
//     {
//         gizmo->SetHoveredAxis(-1);
//         return;
//     }
//
//     if (pickResult.gizmoAxis != -1)
//     {
//         gizmo->SetHoveredAxis(pickResult.gizmoAxis);
//     }
// }

// void Editor::UpdatePick()
//{
//     InputManager input = engine.GetInputManager();
//     if (!input.GetMouseDown(GLFW_MOUSE_BUTTON_LEFT))
//         return;
//
//     PickResult pickResult = picker->Pick(camera, input.mouseX, input.mouseY);
//     if (!pickResult.hit)
//     {
//         SelectUnit(nullptr);
//         return;
//     }
//
//     if (pickResult.gizmoAxis != -1)
//     {
//         const Vector2 clickPos = Vector2(input.mouseX, input.mouseY);
//         gizmo->StartDragging(clickPos, pickResult.gizmoAxis, camera);
//         return;
//     }
//
//     SelectUnit(pickResult.pickedUnit);
// }

} // namespace URay
