#include "Editor.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Core/Input/Action/InputActionSystem.h>
#include <Core/Log/LogSystem.h>
#include <Core/Performance/PerformanceViewModel.h>
#include <Core/Type/TypeAlias.h>
#include <Editor/Asset/AssetEditorSystem.h>
#include <Editor/Asset/Mesh/MeshPreviewSession.h>
#include <Editor/EditorCamera.h>
#include <Editor/Input/EditorInputActions.h>
#include <Editor/Play/PlaySession.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Settings/EditorSettings.h>
#include <Editor/Viewport/ViewportController.h>
#include <Editor/Visual/VisualSystem.h>
#include <Editor/Widget/Dialog/SaveSceneDialog.h>
#include <Editor/Widget/Docking/DockSystem.h>
#include <Editor/Widget/Docking/DockTabGroup.h>
#include <Editor/Widget/EditorUI.h>
#include <Editor/Widget/Layer/DialogLayer.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Panel/Inspector/InspectSystem.h>
#include <Editor/Widget/Panel/MeshPreviewPanel.h>
#include <Editor/Widget/Popup/ComponentPickerPopup.h>
#include <Editor/Widget/Popup/UnitPopup.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Component/ComponentFactory.h>
#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Component/Render/GridComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Object/ObjectSystem.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>
#include <Engine/UI/UISystem.h>
#include <Platform/Window/Window.h>
#include <Platform/Window/WindowSystem.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/Object/Drawable/LineObject.h>
#include <Render/Rendering/RenderPipeline.h>
#include <Render/Rendering/Renderer.h>
#include <Render/Rendering/Scene/RenderScene.h>
#include <Render/Rendering/Scene/SceneSystem.h>

#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

namespace URay
{

Editor* gEditor;

Editor::Editor() = default;

Editor::~Editor() = default;

bool Editor::Initialize()
{
    gEditor = this;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    VirtualFileSystem& filesystem = assetSystem.GetFileSystem();

    editorSettings = std::make_unique<EditorSettings>(filesystem);

    assetEditorSystem = std::make_unique<AssetEditorSystem>(assetSystem.GetDatabase());

    selectionSystem = std::make_unique<SelectionSystem>();
    if (!selectionSystem->Initialize())
    {
        URAY_LOG("[Editor] Failed to initialize selection system.");
        Finalize();
        return false;
    }

    inspectSystem = std::make_unique<InspectSystem>(sceneSystem);
    if (!inspectSystem->Initialize())
    {
        URAY_LOG("[Editor] Failed to initialize inspect system.");
        Finalize();
        return false;
    }

    selectionSystem->Register("ActiveSelectionChanged", this, &Editor::OnActiveSelectionChanged);

    visualSystem = std::make_unique<VisualSystem>(*gEngine, *selectionSystem);
    if (!visualSystem->Initialize())
    {
        URAY_LOG("[Editor] Failed to initialize visual system.");
        Finalize();
        return false;
    }

    playSession = std::make_unique<PlaySession>(sceneSystem);

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    viewportController = std::make_unique<ViewportController>(
        renderSystem.GetRenderer(),
        renderSystem.GetResourceManager(),
        renderSystem.GetSceneSystem(),
        gEngine->GetWindowSystem().GetMainWindow(),
        gEngine->GetInputActionSystem(),
        *selectionSystem,
        *playSession,
        useEditorCamera);

    performanceViewModel = std::make_unique<PerformanceViewModel>();
    renderSystem.GetRenderer().SetPerformanceViewModel(performanceViewModel.get());

    EditorSettingsContext settingsContext;
    const bool settingsLoaded = editorSettings->Load(settingsContext);

    editorUI = std::make_unique<EditorUI>(*viewportController);
    if (!editorUI->Initialize(gEngine->GetUISystem(), settingsContext.layout))
    {
        URAY_LOG("[Editor] Failed to initialize widget system.");
        Finalize();
        return false;
    }

    auto createEmptyGameScene = [&]()
    {
        Scene* gameScene = sceneSystem.LoadScene(sceneSystem.CreateScene(SceneType::Game, ""));

        auto unit = std::make_unique<Unit>();
        unit->SetName("New Unit");
        gameScene->AddUnit(std::move(unit));
    };

    if (settingsLoaded)
    {
        EditorView& editorView = viewportController->GetEditorCamera().GetView();
        editorView.position = settingsContext.cameraSettings.position;
        editorView.rotation = settingsContext.cameraSettings.rotation;

        const VirtualPath& path = settingsContext.startScenePath;
        const String sceneText = filesystem.ReadText(path);

        if (path.ToString().empty() || sceneText.empty())
        {
            URAY_LOG("[Editor] Failed to read start scene: %s", path.ToString().c_str());
            createEmptyGameScene();
        }
        else
        {
            try
            {
                YAML::Node sceneNode = YAML::Load(sceneText);
                auto scene = sceneSystem.CreateScene(SceneType::Game, path);
                scene->Deserialize(sceneNode);

                sceneSystem.LoadScene(std::move(scene));
            }
            catch (const YAML::Exception& exception)
            {
                URAY_LOG("[Editor] Failed to load start scene \'%s\': %s", path.ToString().c_str(), exception.what());
                createEmptyGameScene();
            }
        }
    }
    else
    {
        createEmptyGameScene();
    }

    inputActionMap = CreateEditorInputActionMap();
    if (!inputActionMap)
    {
        URAY_LOG("[Editor] Failed to create input action map.");
        Finalize();
        return false;
    }

    gEngine->GetInputActionSystem().SetActionMap(inputActionMap.get());

    PrepareGrid();

    if (!assetEditorSystem->Register(
            Mesh::StaticType(),
            [this](const AssetHandle& handle)
            {
                OpenMeshPreview(handle);
            }))
    {
        URAY_LOG("[Editor] Failed to register the mesh preview handler.");
        return false;
    }

    return true;
}

void Editor::Finalize()
{
    gEngine->GetInputActionSystem().SetActionMap(nullptr);
    inputActionMap.reset();

    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    auto gameScenes = sceneSystem.GetScenes(SceneType::Game);

    Scene* gameScene = gameScenes.empty() ? nullptr : gameScenes[0];

    EditorView editorView = viewportController->GetEditorCamera().GetView();

    if (editorSettings && editorUI && gameScene)
    {
        EditorSettingsContext settingsContext = {
            .startScenePath = gameScene->GetFilePath().ToString(),
            .cameraSettings = {
                .position = editorView.position,
                .rotation = editorView.rotation,
            },
        };

        if (!editorUI->SaveLayout(settingsContext.layout))
        {
            URAY_LOG("[Editor] Failed to save editor layout.");
        }
        if (!editorSettings->Save(settingsContext))
        {
            URAY_LOG("[Editor] Failed to save editor settings.");
        }
    }

    if (editorUI)
    {
        gEngine->GetUISystem().ClearRootWidget();
        editorUI.reset();
    }

    if (assetEditorSystem)
    {
        assetEditorSystem.reset();
    }

    if (performanceViewModel)
    {
        gEngine->GetRenderSystem().GetRenderer().SetPerformanceViewModel(nullptr);
        performanceViewModel.reset();
    }
    lastProfileFrameNumber = 0;

    viewportController.reset();

    if (playSession)
    {
        playSession->Stop();
        playSession.reset();
    }

    if (visualSystem)
    {
        visualSystem->Finalize();
        visualSystem.reset();
    }

    if (selectionSystem)
        selectionSystem->UnregisterAll(this);

    if (inspectSystem)
    {
        inspectSystem->Finalize();
        inspectSystem.reset();
    }

    if (selectionSystem)
    {
        selectionSystem->Finalize();
        selectionSystem.reset();
    }

    if (editorSettings)
    {
        editorSettings.reset();
    }

    gEditor = nullptr;
}

void Editor::Update()
{
    UpdatePerformanceViewModel();

    URAY_PROFILE_SCOPE("Editor::Update")

    DockSystem& dockSystem = editorUI->GetDockSystem();
    dockSystem.ProcessPendingMove();
    dockSystem.ProcessPendingCleanup();

    assetEditorSystem->ProcessPendingRequests();

    ProcessInputActions();
}

void Editor::ProcessInputActions()
{
    const InputActionSnapshot& actions =
        gEngine->GetInputActionSystem().GetSnapshot();

    const auto wasPressed = [&actions](InputActionId id)
    {
        const InputActionValue* value = actions.FindAction(id);
        return value && value->pressed;
    };

    if (gEngine->GetUISystem().IsTextInputActive())
        return;

    if (wasPressed(EditorInputAction::SaveScene))
    {
        SaveCurrentScene();
    }

    if (wasPressed(EditorInputAction::DeleteSelection))
    {
        DeleteSelectedUnits();
    }
}

bool Editor::SaveCurrentScene()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    auto gameScenes = sceneSystem.GetScenes(SceneType::Game);

    Scene* gameScene = gameScenes.empty() ? nullptr : gameScenes[0];

    if (!gameScene)
        return false;

    if (gameScene->GetFilePath().ToString().empty())
    {
        OpenSaveSceneDialog();
        return true;
    }

    return SaveSceneToPath(*gameScene, gameScene->GetFilePath());
}

bool Editor::SaveSceneToPath(Scene& scene, const VirtualPath& path)
{
    VirtualFileSystem& fileSystem = gEngine->GetAssetSystem().GetFileSystem();

    const String sceneText = YAML::Dump(scene.Serialize());

    if (!fileSystem.WriteText(path, sceneText))
    {
        URAY_LOG("Failed to save scene: %s", path.ToString().c_str());
        return false;
    }

    scene.SetFilePath(path);
    URAY_LOG("Scene saved: %s", path.ToString().c_str());

    return true;
}

void Editor::OpenSaveSceneDialog()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    VirtualFileSystem& fileSystem = gEngine->GetAssetSystem().GetFileSystem();

    auto gameScenes = sceneSystem.GetScenes(SceneType::Game);
    Scene* gameScene = gameScenes.empty() ? nullptr : gameScenes[0];

    SaveSceneDialog* dialog = editorUI->GetDialogLayer().OpenDialog<SaveSceneDialog>(fileSystem);

    const ObjectId sceneId = gameScene ? gameScene->Object::GetId() : ObjectId{};
    dialog->Register("Closed", this, &Editor::OnSaveSceneDialogClosed, dialog, sceneId);
}

void Editor::OnSaveSceneDialogClosed(DialogResult result, SaveSceneDialog* dialog, ObjectId sceneId)
{
    if (result != DialogResult::Accepted)
        return;

    Object* object = gEngine->GetObjectSystem().Resolve(sceneId);
    Scene* gameScene = Cast<Scene>(object);
    if (gameScene && dialog)
        SaveSceneToPath(*gameScene, dialog->GetSavePath());

    editorUI->GetDialogLayer().CloseDialog();
}

void Editor::OnUnitPopupSelected(UnitPopupSelection selection, bool createChild, ObjectId unitId)
{
    if (selection != UnitPopupSelection::EmptyUnit)
        return;

    Unit* targetUnit = nullptr;
    Scene* scene = nullptr;

    if (createChild)
    {
        targetUnit = Cast<Unit>(gEngine->GetObjectSystem().Resolve(unitId));
        if (targetUnit)
            scene = targetUnit->GetOwner();
    }
    else
    {
        auto scenes = gEngine->GetSceneSystem().GetScenes(SceneType::Game);
        scene = scenes.empty() ? nullptr : scenes[0];
    }

    if (scene)
    {
        Unit* createdUnit = scene->CreateEmptyUnit(createChild ? "New Unit" : "Empty Unit", targetUnit);
        selectionSystem->SetSelection(createdUnit);
    }

    editorUI->GetPopupLayer().ClosePopup();
}

void Editor::OnComponentPopupSelected(ComponentPickerSelection selection, ObjectId unitId)
{
    Unit* unit = Cast<Unit>(gEngine->GetObjectSystem().Resolve(unitId));
    if (unit)
    {
        auto component = ComponentFactory::Create(selection.componentTypeName);
        if (component)
            unit->AddComponent(std::move(component));
    }

    editorUI->GetPopupLayer().ClosePopup();
}

void Editor::DeleteSelectedUnits()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    auto gameScenes = sceneSystem.GetScenes(SceneType::Game);
    Scene* gameScene = gameScenes.empty() ? nullptr : gameScenes[0];

    if (!gameScene)
        return;

    const std::vector<Unit*> selectedUnits = selectionSystem->GetSelectedUnits();

    std::unordered_set<Unit*> selectedGameUnits;

    for (Unit* unit : selectedUnits)
    {
        if (!unit || unit->GetOwner() != gameScene)
            continue;

        selectedGameUnits.insert(unit);
    }

    std::vector<Unit*> selectedRoots;

    for (Unit* unit : selectedUnits)
    {
        if (!selectedGameUnits.contains(unit))
            continue;

        bool hasSelectedAncestor = false;

        for (Unit* ancestor = unit->GetParent(); ancestor; ancestor = ancestor->GetParent())
        {
            if (selectedGameUnits.contains(ancestor))
            {
                hasSelectedAncestor = true;
                break;
            }
        }

        if (!hasSelectedAncestor)
        {
            selectedRoots.push_back(unit);
        }
    }

    for (Unit* root : selectedRoots)
    {
        gameScene->DestroyUnit(root);
    }
}

void Editor::OpenMeshPreview(const AssetHandle& meshHandle)
{
    AssetDatabase& database = gEngine->GetAssetSystem().GetDatabase();
    Mesh* mesh = database.Find<Mesh>(meshHandle);
    if (!mesh)
    {
        URAY_LOG("[Editor] Mesh preview asset was not found.");
        return;
    }

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    auto session = std::make_unique<MeshPreviewSession>(
        gEngine->GetSceneSystem(),
        database,
        renderSystem.GetSceneSystem(),
        renderSystem.GetDevice());

    constexpr Extent2D initialExtent = { 800, 600 };
    if (!session->Initialize(meshHandle, initialExtent))
    {
        URAY_LOG("[Editor] Failed to initialize the mesh preview session.");
        return;
    }

    WindowDesc windowDesc = {};
    windowDesc.title = mesh->GetName();
    windowDesc.width = 800;
    windowDesc.height = 600;

    Window* window = gEngine->CreateAdditionalWindow(windowDesc);
    if (!window)
    {
        URAY_LOG("[Editor] Failed to create the mesh preview window.");
        return;
    }

    UISystem& uiSystem = gEngine->GetUISystem();
    auto group = std::make_unique<DockTabGroup>(uiSystem);
    group->AddTab(
        mesh->GetName(),
        std::make_unique<MeshPreviewPanel>(std::move(session)));

    std::unique_ptr<Widget> root = std::move(group);
    if (!uiSystem.SetAdditionalWindowRoot(*window, root))
    {
        URAY_LOG("[Editor] Failed to attach the mesh preview UI.");
        if (!gEngine->DestroyAdditionalWindow(*window))
            URAY_LOG("[Editor] Failed to destroy the mesh preview window.");
        return;
    }
}

void Editor::BuildAdditionalWindowRenderRequest(
    Window& window, Render::RenderRequest& request) const
{
    Widget* root = gEngine->GetUISystem().FindAdditionalWindowRoot(window);
    DockTabGroup* group = root ? Cast<DockTabGroup>(root) : nullptr;
    if (!group)
        return;

    const std::optional<ObjectId> activeId = group->GetActiveTabId();
    if (!activeId)
        return;

    const DockedTab* tab = group->FindTab(*activeId);
    if (!tab)
        return;

    MeshPreviewPanel* panel = Cast<MeshPreviewPanel>(tab->panel);
    if (!panel)
        return;

    auto sceneInfo = panel->GetSession().BuildSceneRenderInfo();
    if (sceneInfo)
        request.sceneRenderInfos.push_back(std::move(*sceneInfo));
}

Render::RenderRequest Editor::BuildRenderRequest() const
{
    Render::RenderRequest request = {};
    Render::SceneRenderInfo sceneRenderInfo = {};
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::SceneSystem& renderSceneSystem = renderSystem.GetSceneSystem();

    EditorView editorView = viewportController->GetEditorCamera().GetView();

    if (playSession->IsPlaying())
    {
        CameraComponent* camera = nullptr;

        Scene* playScene = playSession->GetScene();

        Render::RenderScene* renderScene = renderSceneSystem.GetRenderScene(playScene);
        sceneRenderInfo.scenes.push_back(renderScene);

        for (const auto& unit : playScene->GetUnits())
        {
            for (const auto& comp : unit->GetComponents())
            {
                if (CameraComponent* cameraComp = Cast<CameraComponent>(comp.get()))
                {
                    camera = cameraComp;
                }
            }
        }

        if (!camera || useEditorCamera)
        {
            sceneRenderInfo.view = {
                .cameraPosition = editorView.position,
                .cameraDirection = editorView.GetForward(),
                .nearPlane = editorView.nearPlane,
                .farPlane = editorView.farPlane,
                .viewMatrix = editorView.GetViewMatrix(),
                .projMatrix = editorView.GetProjMatrix(viewportController->GetTargetExtent()),
                .viewMode = viewportController->GetViewMode()
            };
        }
        else
        {
            camera->SetViewportExtent(viewportController->GetTargetExtent());

            TransformComponent* transform = camera->GetOwner()->GetTransform();

            sceneRenderInfo.view = {
                .cameraPosition = transform->GetWorldPosition(),
                .cameraDirection = transform->GetWorldForward(),
                .nearPlane = camera->GetNearPlane(),
                .farPlane = camera->GetFarPlane(),
                .viewMatrix = camera->GetViewMatrix(),
                .projMatrix = camera->GetProjMatrix(),
                .viewMode = viewportController->GetViewMode()
            };
        }

        sceneRenderInfo.selectedUnits = selectionSystem->GetSelectedUnits();
    }
    else
    {
        for (const auto& scene : sceneSystem.GetScenes())
        {
            if (scene->GetSceneType() == SceneType::Preview)
                continue;

            Render::RenderScene* renderScene = renderSceneSystem.GetRenderScene(scene.get());
            sceneRenderInfo.scenes.push_back(renderScene);
        }

        sceneRenderInfo.view = {
            .cameraPosition = editorView.position,
            .cameraDirection = editorView.GetForward(),
            .nearPlane = editorView.nearPlane,
            .farPlane = editorView.farPlane,
            .viewMatrix = editorView.GetViewMatrix(),
            .projMatrix = editorView.GetProjMatrix(viewportController->GetTargetExtent()),
            .viewMode = viewportController->GetViewMode()
        };

        sceneRenderInfo.selectedUnits = selectionSystem->GetSelectedUnits();
    }

    sceneRenderInfo.targets = renderSystem.GetRenderer().GetViewportSceneRenderTargets();

    request.sceneRenderInfos.push_back(std::move(sceneRenderInfo));
    request.uiDrawContext = &gEngine->GetUISystem().GetDrawContext();

    return request;
}

void Editor::LoadGameScene(const VirtualPath& scenePath)
{
    if (playSession->IsPlaying())
    {
        URAY_LOG("[Editor] Cannot load a game scene while playing.");
        return;
    }

    VirtualFileSystem& fileSystem = gEngine->GetAssetSystem().GetFileSystem();
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();

    const String sceneText = fileSystem.ReadText(scenePath);
    if (sceneText.empty())
    {
        URAY_LOG("[Editor] Failed to read scene: %s", scenePath.ToString().c_str());
        return;
    }

    std::unique_ptr<Scene> newScene = nullptr;

    try
    {
        YAML::Node sceneNode = YAML::Load(sceneText);

        newScene = sceneSystem.CreateScene(SceneType::Game, scenePath);
        newScene->Deserialize(sceneNode);
    }
    catch (const YAML::Exception& exception)
    {
        URAY_LOG(
            "[Editor] Failed to load game scene '%s': %s",
            scenePath.ToString().c_str(),
            exception.what());
        return;
    }

    selectionSystem->ClearSelection();
    inspectSystem->ClearTarget();

    const std::vector<Scene*> gameScenes = sceneSystem.GetScenes(SceneType::Game);

    for (Scene* scene : gameScenes)
        sceneSystem.UnloadScene(scene->GetId());

    sceneSystem.LoadScene(std::move(newScene));
}

void Editor::PrepareGrid()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    Render::SceneSystem& renderSceneSystem = gEngine->GetRenderSystem().GetSceneSystem();

    auto gameScenes = sceneSystem.GetScenes(SceneType::Game);
    if (gameScenes.empty())
        return;

    Scene* gameScene = gameScenes[0];
    if (!gameScene)
        return;

    Render::RenderScene* renderGameScene = renderSceneSystem.GetRenderScene(gameScene);
    if (!renderGameScene)
        return;

    Render::Line xCoordLine;
    xCoordLine.start = Vector3(-100.0f, 0.0f, 0.0f);
    xCoordLine.end = Vector3(100.0f, 0.0f, 0.0f);
    xCoordLine.color = Color::Red;

    Render::Line yCoordLine;
    yCoordLine.start = Vector3(0.0f, -100.0f, 0.0f);
    yCoordLine.end = Vector3(0.0f, 100.0f, 0.0f);
    yCoordLine.color = Color::Green;

    Render::Line zCoordLine;
    zCoordLine.start = Vector3(0.0f, 0.0f, -100.0f);
    zCoordLine.end = Vector3(0.0f, 0.0f, 100.0f);
    zCoordLine.color = Color::Blue;

    Render::LineObjectState lineObjectState;
    lineObjectState.lines = { xCoordLine,
                              yCoordLine,
                              zCoordLine };

    auto lineObject = std::make_unique<Render::LineObject>(lineObjectState);

    renderSceneSystem.AddObject(*renderGameScene, std::move(lineObject), nullptr);
}

void Editor::UpdatePerformanceViewModel()
{
    PerformanceAnalytics& analytics = gEngine->GetPerformanceAnalytics();

    const uint64 completedFrameNumber = analytics.GetCompletedFrameNumber();

    if (completedFrameNumber == 0 || completedFrameNumber == lastProfileFrameNumber)
        return;

    performanceViewModel->AddCPUFrame(
        completedFrameNumber,
        analytics.GetCompletedSamples());

    lastProfileFrameNumber = completedFrameNumber;
}

void Editor::OnActiveSelectionChanged(Unit* previousUnit, Unit* activeUnit)
{
    if (activeUnit)
        inspectSystem->SetTarget(InspectTarget::FromUnit(activeUnit));
    else
        inspectSystem->ClearTarget();
}

} // namespace URay
