#include "Editor.h"

#include "Editor/Selection/SelectionSystem.h"
#include "Editor/Settings/EditorSettings.h"
#include "Editor/Visual/VisualSystem.h"
#include "Editor/Widget/Viewport/ViewportWidget.h"
#include "Editor/Widget/WidgetSystem.h"

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

#include "Render/RenderPass/UIPass.h"
#include "Render/RenderPipeline.h"
#include "Render/RenderSystem.h"
#include "Render/Renderer.h"

#include <imgui/imgui.h>

#include <memory>

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
    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();

    if (!renderSystem.InitializeImGui())
        return false;

    editorSettings = std::make_unique<EditorSettings>(filesystem);

    editorCamera = &PrepareEditorScene();

    selectionSystem = std::make_unique<SelectionSystem>();
    if (!selectionSystem->Initialize())
        return false;

    visualSystem = std::make_unique<VisualSystem>(*gEngine, *selectionSystem);
    if (!visualSystem->Initialize())
        return false;

    widgetSystem = std::make_unique<WidgetSystem>();
    if (!widgetSystem->Initialize())
        return false;
    widgetSystem->GetViewport().SetCamera(editorCamera);

    EditorSettingsContext settingsContext = {
        .rootWidget = widgetSystem->GetRootWidget()
    };

    if (editorSettings->Load(settingsContext))
    {
        TransformComponent* cameraTransform = editorCamera->GetOwner()->GetTransform();
        cameraTransform->SetPosition(settingsContext.cameraSettings.position);
        cameraTransform->SetRotation(settingsContext.cameraSettings.rotation);

        const std::string sceneText = filesystem.ReadText(settingsContext.startScenePath);
        YAML::Node sceneNode = YAML::Load(sceneText);

        std::unique_ptr<Scene> loadedScene = sceneSystem.CreateScene(SceneType::Game, settingsContext.startScenePath);
        loadedScene->Deserialize(sceneNode);

        sceneSystem.SwitchScene(std::move(loadedScene));
    }
    else
    {
        std::unique_ptr<Scene> loadedScene = sceneSystem.CreateScene(SceneType::Game, "");

        sceneSystem.SwitchScene(std::move(loadedScene));
    }

    return true;
}

void Editor::Finalize()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();

    TransformComponent* cameraTransform = editorCamera->GetOwner()->GetTransform();

    EditorSettingsContext settingsContext = {
        .rootWidget = widgetSystem->GetRootWidget(),
        .startScenePath = sceneSystem.GetSceneByType(SceneType::Game)->GetFilePath().ToString(),
        .cameraSettings = {
            .position = cameraTransform->GetPosition(),
            .rotation = cameraTransform->GetRotation(),
        },
    };

    editorSettings->Save(settingsContext);

    widgetSystem->Finalize();
    widgetSystem.reset();

    visualSystem->Finalize();
    visualSystem.reset();

    selectionSystem->Finalize();
    selectionSystem.reset();

    renderSystem.FinalizeImGui();
}

void Editor::Update()
{
    URAY_PROFILE_SCOPE("Editor::Update")

    Timer& timer = gEngine->GetTimer();
    float deltaTime = timer.GetDeltaTime();

    widgetSystem->Update(deltaTime);
}

void Editor::PrepareRender()
{
    URAY_PROFILE_SCOPE("Editor::PrepareRender")

    widgetSystem->PrepareRender();
}

void Editor::StartGame()
{
    if (isPlaying)
        return;

    isPlaying = true;
    useEditorCamera = false;

    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    Scene* gameScene = sceneSystem.GetSceneByType(SceneType::Game);

    YAML::Node gameSceneNode = gameScene->Serialize();

    std::unique_ptr<Scene> playScene = sceneSystem.CreateScene(SceneType::Play, "");
    playScene->Deserialize(gameSceneNode);

    sceneSystem.LoadScene(std::move(playScene));
}

void Editor::StopGame()
{
    if (!isPlaying)
        return;

    isPlaying = false;

    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    sceneSystem.UnloadScene(SceneType::Play);
}

Render::RenderRequest Editor::BuildRenderRequest() const
{
    Render::RenderRequest request = {};
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();

    ViewportWidget& viewport = widgetSystem->GetViewport();

    if (isPlaying)
    {
        CameraComponent* camera = nullptr;

        for (const auto& scene : sceneSystem.GetScenes())
        {
            if (scene->GetType() == SceneType::Play)
            {
                request.scenes.push_back(scene->GetRenderScene());

                for (const auto& unit : scene->GetUnits())
                {
                    for (Component* comp : unit->GetComponents())
                    {
                        if (CameraComponent* cameraComp = Cast<CameraComponent>(comp))
                        {
                            camera = cameraComp;
                        }
                    }
                }
            }
        }

        if (useEditorCamera)
        {
            camera = editorCamera;
        }

        camera->SetViewportExtent(
            { .width = viewport.GetTargetExtent().width,
              .height = viewport.GetTargetExtent().height });

        request.view = {
            .viewMatrix = camera ? camera->GetViewMatrix() : Matrix::Identity,
            .projMatrix = camera ? camera->GetProjMatrix() : Matrix::Identity,
            .viewMode = viewport.GetViewMode()
        };
    }
    else
    {
        for (const auto& scene : sceneSystem.GetScenes())
        {
            request.scenes.push_back(scene->GetRenderScene());
        }

        request.view = {
            .viewMatrix = editorCamera->GetViewMatrix(),
            .projMatrix = editorCamera->GetProjMatrix(),
            .viewMode = viewport.GetViewMode()
        };
    }

    return request;
}

CameraComponent& Editor::PrepareEditorScene()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    std::unique_ptr<Scene> editorScene = sceneSystem.CreateScene(SceneType::Editor, "");

    std::unique_ptr<Unit> cameraUnit = std::make_unique<Unit>();
    cameraUnit->SetName("Editor Camera");

    TransformComponent* cameraTransform = new TransformComponent();
    cameraTransform->SetPosition(Vector3(0.0f, -5.0f, 0.0f));
    CameraComponent* camera = new CameraComponent();
    cameraUnit->AddComponent(cameraTransform);
    cameraUnit->AddComponent(camera);

    std::unique_ptr<Unit> gridUnit = std::make_unique<Unit>();
    gridUnit->SetName("Grid");
    GridComponent* gridComponent = new GridComponent();
    gridUnit->AddComponent(gridComponent);

    editorScene->AddUnit(std::move(cameraUnit));
    editorScene->AddUnit(std::move(gridUnit));

    sceneSystem.LoadScene(std::move(editorScene));

    return *camera;
}

} // namespace URay
