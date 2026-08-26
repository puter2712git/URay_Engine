#include "Editor.h"

#include "Editor/EditorPicker.h"
#include "Editor/GizmoController.h"
#include "Editor/Input/UIInputRouter.h"
#include "Editor/Settings/EditorSettings.h"
#include "Editor/Widget/Console/ConsoleWidget.h"
#include "Editor/Widget/Filesystem/FilesystemWidget.h"
#include "Editor/Widget/InspectorWidget.h"
#include "Editor/Widget/MainMenuBarWidget.h"
#include "Editor/Widget/SceneTreeWidget.h"
#include "Editor/Widget/Splitter.h"
#include "Editor/Widget/StatusWidget.h"
#include "Editor/Widget/ViewportWidget.h"
#include "Editor/Widget/Widget.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Component/Render/GridComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/File/VirtualPath.h"
#include "Core/Timer.h"

#include "Render/RenderPipeline.h"
#include "Render/RenderSystem.h"
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
    Render::RenderSystem& renderSystem = engine.GetRenderSystem();

    if (!renderSystem.InitializeImGui(engine.GetAssetSystem().GetFilesystem()))
        return false;

    inputRouter = std::make_unique<UIInputRouter>(engine.GetWindow());
    editorSettings = std::make_unique<EditorSettings>(engine.GetAssetSystem().GetFilesystem());

    editorCamera = &PrepareEditorScene();

    mainMenuBarWidget = std::make_unique<MainMenuBarWidget>(engine);

    std::unique_ptr<SceneTreeWidget> sceneTree = std::make_unique<SceneTreeWidget>(*this, engine);
    std::unique_ptr<InspectorWidget> inspector = std::make_unique<InspectorWidget>(*this);
    std::unique_ptr<ConsoleWidget> console = std::make_unique<ConsoleWidget>();
    std::unique_ptr<FilesystemWidget> filesystem = std::make_unique<FilesystemWidget>(engine.GetAssetSystem().GetFilesystem());
    std::unique_ptr<StatusWidget> status = std::make_unique<StatusWidget>(engine);
    std::unique_ptr<ViewportWidget> viewport = std::make_unique<ViewportWidget>(renderSystem.GetRenderer(), *editorCamera, engine, [this](Unit* unit)
                                                                                { SelectUnit(unit); });
    viewportWidget = viewport.get();

    std::unique_ptr<Splitter> rightPanel2 = std::make_unique<Splitter>("RightPanel2", SplitAxis::Vertical, std::move(sceneTree), std::move(inspector));
    std::unique_ptr<Splitter> rightPanel = std::make_unique<Splitter>("RightPanel", SplitAxis::Vertical, std::move(status), std::move(rightPanel2));
    std::unique_ptr<Splitter> leftPanel2 = std::make_unique<Splitter>("LeftPanel2", SplitAxis::Horizontal, std::move(console), std::move(filesystem));
    std::unique_ptr<Splitter> leftPanel = std::make_unique<Splitter>("LeftPanel", SplitAxis::Vertical, std::move(viewport), std::move(leftPanel2));

    rootWidget = std::make_unique<Splitter>(
        "Root",
        SplitAxis::Horizontal,
        std::move(leftPanel),
        std::move(rightPanel));

    EditorSettingsContext settingsContext = {
        .rootWidget = *rootWidget
    };

    if (editorSettings->Load(settingsContext))
    {
        TransformComponent* cameraTransform = editorCamera->GetOwner()->GetTransform();
        cameraTransform->SetPosition(settingsContext.cameraSettings.position);
        cameraTransform->SetRotation(settingsContext.cameraSettings.rotation);

        AssetSystem& assetSystem = engine.GetAssetSystem();
        VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

        const std::string sceneText = filesystem.ReadText(settingsContext.startScenePath);
        YAML::Node sceneNode = YAML::Load(sceneText);

        std::unique_ptr<Scene> loadedScene = std::make_unique<Scene>(SceneType::Game, settingsContext.startScenePath);
        loadedScene->Deserialize(sceneNode);

        SceneSystem& sceneSystem = engine.GetSceneSystem();
        sceneSystem.SwitchScene(std::move(loadedScene));
    }
    else
    {
        std::unique_ptr<Scene> loadedScene = std::make_unique<Scene>(SceneType::Game, "");

        SceneSystem& sceneSystem = engine.GetSceneSystem();
        sceneSystem.SwitchScene(std::move(loadedScene));
    }

    return true;
}

void Editor::Finalize()
{
    SceneSystem& sceneSystem = engine.GetSceneSystem();

    TransformComponent* cameraTransform = editorCamera->GetOwner()->GetTransform();

    EditorSettingsContext settingsContext = {
        .rootWidget = *rootWidget,
        .startScenePath = sceneSystem.GetSceneByType(SceneType::Game)->GetFilePath().ToString(),
        .cameraSettings = {
            .position = cameraTransform->GetPosition(),
            .rotation = cameraTransform->GetRotation(),
        },
    };

    editorSettings->Save(settingsContext);

    mainMenuBarWidget.reset();
    rootWidget.reset();

    Render::RenderSystem& renderSystem = engine.GetRenderSystem();
    renderSystem.FinalizeImGui();
}

void Editor::Update()
{
    URAY_PROFILE_SCOPE("Editor::Update")

    Timer& timer = engine.GetTimer();
    float deltaTime = timer.GetDeltaTime();

    inputRouter->Process(*rootWidget, engine.GetInputManager());

    mainMenuBarWidget->Update(deltaTime);
    rootWidget->Update(deltaTime);
}

void Editor::PrepareRender()
{
    URAY_PROFILE_SCOPE("Editor::PrepareRender")

    Render::Renderer& renderer = engine.GetRenderSystem().GetRenderer();
    Render::DrawCommandBuilder& builder = engine.GetRenderSystem().GetPipeline().GetBuilder();

    rootWidget->PrepareRender(builder);

    renderer.BeginImGui();

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
    std::unique_ptr<Scene> editorScene = std::make_unique<Scene>(SceneType::Editor, "");

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

    SceneSystem& sceneSystem = engine.GetSceneSystem();
    sceneSystem.LoadScene(std::move(editorScene));

    return *camera;
}

} // namespace URay
