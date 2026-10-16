#include "Editor.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Core/Input/Action/InputActionSystem.h>
#include <Core/Log/LogSystem.h>
#include <Core/Performance/PerformanceViewModel.h>
#include <Core/TimeFlow/Timer.h>
#include <Editor/Input/EditorInputActions.h>
#include <Editor/Play/PlaySession.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Settings/EditorSettings.h>
#include <Editor/Viewport/ViewportController.h>
#include <Editor/Visual/VisualSystem.h>
#include <Editor/Widget/Dialog/SaveSceneDialog.h>
#include <Editor/Widget/Layer/DialogLayer.h>
#include <Editor/Widget/Panel/Inspector/InspectSystem.h>
#include <Editor/Widget/WidgetSystem.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Component/Render/GridComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>
#include <Engine/Script/ScriptSystem.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/RenderPass/UIPass.h>
#include <Render/Rendering/RenderPipeline.h>
#include <Render/Rendering/Renderer.h>
#include <Render/Rendering/Scene/SceneSystem.h>

#include <memory>
#include <unordered_set>
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

    editorCamera = &PrepareEditorScene();

    selectionSystem = std::make_unique<SelectionSystem>();
    if (!selectionSystem->Initialize())
    {
        URAY_LOG("[Editor] Failed to initialize selection system.");
        Finalize();
        return false;
    }

    inspectSystem = std::make_unique<InspectSystem>();

    selectionSystem->GetOnActiveSelectionChangedRay().Register(
        this,
        [this](Unit*, Unit* activeUnit)
        {
            if (activeUnit)
                inspectSystem->SetTarget(InspectTarget::FromUnit(activeUnit));
            else
                inspectSystem->ClearTarget();
        });

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
        gEngine->GetWindow(),
        gEngine->GetInputActionSystem(),
        *selectionSystem,
        *playSession,
        *editorCamera,
        useEditorCamera);

    performanceViewModel = std::make_unique<PerformanceViewModel>();
    renderSystem.GetRenderer().SetPerformanceViewModel(performanceViewModel.get());

    widgetSystem = std::make_unique<WidgetSystem>(*viewportController);
    if (!widgetSystem->Initialize())
    {
        URAY_LOG("[Editor] Failed to initialize widget system.");
        Finalize();
        return false;
    }

    EditorSettingsContext settingsContext = {
        .rootWidget = widgetSystem->GetRootWidget()
    };

    auto createEmptyGameScene = [&]()
    {
        sceneSystem.SwitchScene(sceneSystem.CreateScene(SceneType::Game, ""));
        Scene* gameScene = sceneSystem.GetSceneByType(SceneType::Game);

        auto unit = std::make_unique<Unit>();
        unit->SetName("New Unit");
        gameScene->AddUnit(std::move(unit));
    };

    if (editorSettings->Load(settingsContext))
    {
        TransformComponent* cameraTransform = editorCamera->GetOwner()->GetTransform();
        cameraTransform->SetLocalPosition(settingsContext.cameraSettings.position);
        cameraTransform->SetLocalRotation(settingsContext.cameraSettings.rotation);

        const VirtualPath& path = settingsContext.startScenePath;
        const std::string sceneText = filesystem.ReadText(path);

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
                sceneSystem.SwitchScene(std::move(scene));
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

    return true;
}

void Editor::Finalize()
{
    gEngine->GetInputActionSystem().SetActionMap(nullptr);
    inputActionMap.reset();

    SceneSystem& sceneSystem = gEngine->GetSceneSystem();

    Scene* gameScene = sceneSystem.GetSceneByType(SceneType::Game);

    if (editorSettings && editorCamera && widgetSystem && gameScene)
    {
        TransformComponent* cameraTransform = editorCamera->GetOwner()->GetTransform();

        EditorSettingsContext settingsContext = {
            .rootWidget = widgetSystem->GetRootWidget(),
            .startScenePath = gameScene->GetFilePath().ToString(),
            .cameraSettings = {
                .position = cameraTransform->GetLocalPosition(),
                .rotation = cameraTransform->GetLocalRotation(),
            },
        };

        if (!editorSettings->Save(settingsContext))
        {
            URAY_LOG("[Editor] Failed to save editor settings.");
        }
    }

    if (widgetSystem)
    {
        widgetSystem->Finalize();
        widgetSystem.reset();
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
    {
        selectionSystem->Finalize();
        selectionSystem.reset();
    }

    editorCamera = nullptr;

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

    Timer& timer = gEngine->GetTimer();
    float deltaTime = timer.GetDeltaTime();

    widgetSystem->Update(deltaTime);
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

    if (wasPressed(EditorInputAction::ReloadGameScripts))
    {
        gEngine->GetScriptSystem().ReloadGameScripts();
    }

    if (widgetSystem->IsTextInputActive())
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
    Scene* scene = sceneSystem.GetSceneByType(SceneType::Game);

    if (!scene)
        return false;

    if (scene->GetFilePath().ToString().empty())
    {
        OpenSaveSceneDialog();
        return true;
    }

    return SaveSceneToPath(*scene, scene->GetFilePath());
}

bool Editor::SaveSceneToPath(Scene& scene, const VirtualPath& path)
{
    VirtualFileSystem& fileSystem = gEngine->GetAssetSystem().GetFileSystem();

    const std::string sceneText = YAML::Dump(scene.Serialize());

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
    VirtualFileSystem& fileSystem = gEngine->GetAssetSystem().GetFileSystem();

    SaveSceneDialog* dialog = widgetSystem->GetDialogLayer().OpenDialog<SaveSceneDialog>(fileSystem);

    dialog->GetClosedRay().Register(this, [this, dialog](DialogResult result)
                                    {
        if (result == DialogResult::Accepted)
        {
            Scene* scene = gEngine->GetSceneSystem().GetSceneByType(SceneType::Game);
            if (scene)
            {
                SaveSceneToPath(*scene, dialog->GetSavePath());
            }

            widgetSystem->GetDialogLayer().CloseDialog();
        } });
}

void Editor::DeleteSelectedUnits()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    Scene* scene = sceneSystem.GetSceneByType(SceneType::Game);

    if (!scene)
        return;

    const std::vector<Unit*> selectedUnits = selectionSystem->GetSelectedUnits();

    std::unordered_set<Unit*> selectedGameUnits;

    for (Unit* unit : selectedUnits)
    {
        if (!unit || unit->GetOwner() != scene)
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
        scene->DestroyUnit(root);
    }
}

void Editor::PrepareRender()
{
    URAY_PROFILE_SCOPE("Editor::PrepareRender")

    widgetSystem->PrepareRender();
    widgetSystem->Paint();
}

Render::RenderRequest Editor::BuildRenderRequest() const
{
    Render::RenderRequest request = {};
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::SceneSystem& renderSceneSystem = renderSystem.GetSceneSystem();

    if (playSession->IsPlaying())
    {
        CameraComponent* camera = nullptr;

        Scene* playScene = playSession->GetScene();

        Render::RenderScene* renderScene = renderSceneSystem.GetRenderScene(playScene);
        request.scenes.push_back(renderScene);

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
            camera = editorCamera;
        }

        TransformComponent* transform = camera->GetOwner()->GetTransform();

        camera->SetViewportExtent(
            viewportController->GetTargetExtent());

        request.view = {
            .cameraPosition = transform ? transform->GetWorldPosition() : Vector3::Zero,
            .cameraDirection = transform ? transform->GetWorldForward() : Vector3::Forward,
            .nearPlane = camera ? camera->GetNearPlane() : 0.1f,
            .farPlane = camera ? camera->GetFarPlane() : 1000.0f,
            .viewMatrix = camera ? camera->GetViewMatrix() : Matrix::Identity,
            .projMatrix = camera ? camera->GetProjMatrix() : Matrix::Identity,
            .viewMode = viewportController->GetViewMode()
        };

        request.selectedUnits = selectionSystem->GetSelectedUnits();
    }
    else
    {
        for (const auto& scene : sceneSystem.GetScenes())
        {
            Render::RenderScene* renderScene = renderSceneSystem.GetRenderScene(scene.get());
            request.scenes.push_back(renderScene);
        }

        TransformComponent* transform = editorCamera->GetOwner()->GetTransform();

        request.view = {
            .cameraPosition = transform ? transform->GetWorldPosition() : Vector3::Zero,
            .cameraDirection = transform ? transform->GetWorldForward() : Vector3::Forward,
            .nearPlane = editorCamera ? editorCamera->GetNearPlane() : 0.1f,
            .farPlane = editorCamera ? editorCamera->GetFarPlane() : 1000.0f,
            .viewMatrix = editorCamera->GetViewMatrix(),
            .projMatrix = editorCamera->GetProjMatrix(),
            .viewMode = viewportController->GetViewMode()
        };

        request.selectedUnits = selectionSystem->GetSelectedUnits();
    }

    request.uiDrawContext = &widgetSystem->GetDrawContext();

    return request;
}

CameraComponent& Editor::PrepareEditorScene()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    std::unique_ptr<Scene> editorScene = sceneSystem.CreateScene(SceneType::Editor, "");

    std::unique_ptr<Unit> cameraUnit = std::make_unique<Unit>();
    cameraUnit->SetName("Editor Camera");

    std::unique_ptr<TransformComponent> cameraTransform = std::make_unique<TransformComponent>();
    cameraTransform->SetLocalPosition(Vector3(0.0f, -5.0f, 0.0f));
    std::unique_ptr<CameraComponent> camera = std::make_unique<CameraComponent>();
    CameraComponent* cameraPtr = camera.get();
    cameraUnit->AddComponent(std::move(cameraTransform));
    cameraUnit->AddComponent(std::move(camera));

    std::unique_ptr<Unit> gridUnit = std::make_unique<Unit>();
    gridUnit->SetName("Grid");
    std::unique_ptr<GridComponent> gridComponent = std::make_unique<GridComponent>();
    gridUnit->AddComponent(std::move(gridComponent));

    editorScene->AddUnit(std::move(cameraUnit));
    editorScene->AddUnit(std::move(gridUnit));

    sceneSystem.LoadScene(std::move(editorScene));

    return *cameraPtr;
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

} // namespace URay
