#include "Editor/Widget/Panel/Viewport/ViewportWidget.h"

#include "Editor/Editor.h"
#include "Editor/EditorCamera.h"
#include "Editor/Gizmo/TransformGizmo.h"
#include "Editor/Pick/PickSystem.h"
#include "Editor/Selection/SelectionSystem.h"
#include "Editor/Widget/Element/Button.h"
#include "Editor/Widget/Layout/HorizontalLayout.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/LogSystem.h"
#include "Core/Type/Types.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/RHI/Texture/Sampler.h"
#include "Render/RenderSystem.h"
#include "Render/Rendering/Batch/UIDrawContext.h"
#include "Render/Rendering/RenderTargetRegistry.h"
#include "Render/Rendering/Renderer.h"
#include "Render/ResourceManager.h"

#include "Platform/Window/Window.h"

#include <imgui/imgui.h>

namespace URay
{

ViewportWidget::ViewportWidget()
    : renderer(gEngine->GetRenderSystem().GetRenderer()),
      selectionSystem(gEditor->GetSelectionSystem())
{
    SetBackgroundColor(Color(0.1f, 0.1f, 0.1f));

    gizmo = std::make_unique<TransformGizmo>();
    pickSystem = std::make_unique<PickSystem>(*gizmo);
    pickSystem->Initialize();

    editorCamera = std::make_unique<EditorCamera>();

    gEditor->GetSelectionSystem().GetOnSelectionChangedRay().Register(this, [this](Unit*, Unit* unit)
                                                                      { gizmo->SetTarget(unit); });

    renderer.GetObjectIdReadbackRay().Register(this, [this](const Render::ObjectIdReadbackResult& result)
                                               { selectionSystem.SelectUnit(pickSystem->FindTarget(result.objectId)); });

    auto horizontalLayout = std::make_unique<HorizontalLayout>();
    layout = horizontalLayout.get();
    layout->SetPadding(Padding(5.0f, 10.0f));
    layout->SetGap(5.0f);
    children.push_back(std::move(horizontalLayout));

    auto buttonA = std::make_unique<Button>("A");
    buttonA->SetDesiredSize(Vector2(40.0f, 0.0f));
    layout->AddChild(std::move(buttonA));

    auto buttonB = std::make_unique<Button>("B");
    buttonB->SetDesiredSize(Vector2(40.0f, 0.0f));
    layout->AddChild(std::move(buttonB));
}

ViewportWidget::~ViewportWidget()
{
    renderer.GetObjectIdReadbackRay().UnregisterAll(this);
    selectionSystem.GetOnSelectionChangedRay().UnregisterAll(this);

    if (pickSystem)
    {
        pickSystem->Finalize();
        pickSystem.reset();
    }
    if (gizmo)
    {
        gizmo.reset();
        gizmo = nullptr;
    }
}

void ViewportWidget::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    constexpr float toolbarHeight = 40.0f;

    layout->Arrange({ .position = rect.position,
                      .size = Vector2(rect.size.x, toolbarHeight) });
}

EventReply ViewportWidget::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Right)
    {
        isCameraRotating = true;

        return EventReply{
            .requestFocus = true,
            .capturePointer = true,
        };
    }

    if (event.changedButton == MouseButton::Left)
    {
        const auto targetPosition = WindowToRenderTarget(event.position);
        if (!targetPosition)
            return {};

        const PickResult pickResult = pickSystem->Pick(editorCamera->GetCamera(), targetPosition->x, targetPosition->y);
        if (pickResult.isHit && pickResult.gizmoHandle != GizmoHandleId::None)
        {
            gizmo->StartDragging(*targetPosition, pickResult.gizmoHandle, *editorCamera->GetCamera());
        }
        else
        {
            renderer.GetObjectIdReadback().Request(
                static_cast<uint32>(targetPosition->x),
                static_cast<uint32>(targetPosition->y));
        }
    }

    return {
        .requestFocus = true,
        .capturePointer = true,
    };
}

EventReply ViewportWidget::OnPointerMove(const PointerEvent& event)
{
    if (isCameraRotating)
    {
        cameraMouseDelta.x += event.delta.x;
        cameraMouseDelta.y += event.delta.y;
        return {};
    }

    if (gizmo)
    {
        const auto targetPosition = WindowToRenderTarget(event.position);
        if (!targetPosition)
            return {};

        cachedPosition = *targetPosition;
    }

    return {};
}

EventReply ViewportWidget::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Right)
    {
        isCameraRotating = false;
    }

    if (event.changedButton == MouseButton::Left)
    {
        if (gizmo && gizmo->IsDragging())
        {
            gizmo->EndDragging();
        }
    }

    return EventReply{
        .releasePointer = event.pressedButtons == 0,
    };
}

EventReply ViewportWidget::OnKeyDown(const KeyEvent& event)
{
    if (event.key == KeyCode::S &&
        event.action == KeyAction::Pressed &&
        (event.modifiers & ModifierKey::Ctrl) != ModifierKey::None)
    {
        SaveCurrScene();
        return {};
    }

    if (event.key == KeyCode::Delete)
    {
        SceneSystem& sceneSystem = gEngine->GetSceneSystem();
        Scene* currScene = sceneSystem.GetSceneByType(SceneType::Game);

        Unit* selectedUnit = selectionSystem.GetSelectedUnit();

        if (currScene && selectedUnit)
        {
            selectionSystem.SelectUnit(nullptr);
            currScene->DestroyUnit(selectedUnit);
        }
    }

    if (event.key == KeyCode::Space && event.action == KeyAction::Pressed)
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

    if (event.key == KeyCode::GraveAccent && gEditor->IsPlaying())
    {
        gEditor->SetUseEditorCamera(!gEditor->UsingEditorCamera());
    }

    if (event.action == KeyAction::Pressed)
    {
        if (event.key == KeyCode::W)
        {
            cameraMoveAxis = cameraMoveAxis + Vector3::Forward;
        }
        if (event.key == KeyCode::S)
        {
            cameraMoveAxis = cameraMoveAxis - Vector3::Forward;
        }
        if (event.key == KeyCode::D)
        {
            cameraMoveAxis = cameraMoveAxis + Vector3::Right;
        }
        if (event.key == KeyCode::A)
        {
            cameraMoveAxis = cameraMoveAxis - Vector3::Right;
        }
        if (event.key == KeyCode::E)
        {
            cameraMoveAxis = cameraMoveAxis + Vector3::Up;
        }
        if (event.key == KeyCode::Q)
        {
            cameraMoveAxis = cameraMoveAxis - Vector3::Up;
        }
    }

    return {};
}

EventReply ViewportWidget::OnKeyUp(const KeyEvent& event)
{
    if (event.action == KeyAction::Released)
    {
        if (event.key == KeyCode::W)
        {
            cameraMoveAxis = cameraMoveAxis - Vector3::Forward;
        }
        if (event.key == KeyCode::S)
        {
            cameraMoveAxis = cameraMoveAxis + Vector3::Forward;
        }
        if (event.key == KeyCode::D)
        {
            cameraMoveAxis = cameraMoveAxis - Vector3::Right;
        }
        if (event.key == KeyCode::A)
        {
            cameraMoveAxis = cameraMoveAxis + Vector3::Right;
        }
        if (event.key == KeyCode::E)
        {
            cameraMoveAxis = cameraMoveAxis - Vector3::Up;
        }
        if (event.key == KeyCode::Q)
        {
            cameraMoveAxis = cameraMoveAxis + Vector3::Up;
        }
    }

    return {};
}

void ViewportWidget::SetCamera(CameraComponent* camera)
{
    editorCamera->SetCamera(camera);
    camera->SetViewportExtent(GetTargetExtent());
}

void ViewportWidget::OnUpdate(float deltaTime)
{
    PickResult pickResult = pickSystem->Pick(editorCamera->GetCamera(), cachedPosition.x, cachedPosition.y);
    if (!pickResult.isHit || pickResult.gizmoHandle == GizmoHandleId::None)
    {
        gizmo->SetHoveredHandle(GizmoHandleId::None);
    }

    if (pickResult.gizmoHandle != GizmoHandleId::None)
    {
        gizmo->SetHoveredHandle(pickResult.gizmoHandle);
    }

    gizmo->Update(cachedPosition, *editorCamera->GetCamera());

    EditorCameraInput input = {};
    input.horizontalAxis = Vector3(cameraMoveAxis.x, cameraMoveAxis.y, 0.0f);
    input.verticalAxis = Vector3(0.0f, 0.0f, cameraMoveAxis.z);
    input.mouseDelta = cameraMouseDelta;

    editorCamera->Update(deltaTime, input);

    cameraMouseDelta = Vector2::Zero;
}

void ViewportWidget::OnPaint(Render::UIDrawContext& context)
{
    Panel::OnPaint(context);

    constexpr float toolbarHeight = 40.0f;

    imageRect = rect;
    imageRect.position.y += toolbarHeight;
    imageRect.size.y -= toolbarHeight;

    if (imageRect.size.x <= 0.0f || imageRect.size.y <= 0.0f)
        return;

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::Renderer& renderer = renderSystem.GetRenderer();

    Render::RenderTarget& postProcessRT =
        renderer.GetRenderTargetRegistry().Get(
            Render::RenderTargetId::PostProcess);

    Render::TextureView* view = postProcessRT.GetColorView(0);
    Render::Sampler* sampler =
        gEngine->GetRenderSystem()
            .GetResourceManager()
            .GetOrCreateSampler({});

    if (!view || !sampler)
        return;

    targetExtent = postProcessRT.GetExtent();

    Window& window = gEngine->GetWindow();
    const Extent2D clientSize = window.GetClientSize();
    const Extent2D framebufferSize = window.GetFramebufferSize();

    const float scaleX =
        clientSize.width > 0
            ? static_cast<float>(framebufferSize.width) / clientSize.width
            : 1.0f;

    const float scaleY =
        clientSize.height > 0
            ? static_cast<float>(framebufferSize.height) / clientSize.height
            : 1.0f;

    renderer.RequestSceneRenderTargetResize({
        .width = static_cast<uint32>(std::round(imageRect.size.x * scaleX)),
        .height = static_cast<uint32>(std::round(imageRect.size.y * scaleY)),
    });

    editorCamera->GetCamera()->SetViewportExtent(targetExtent);

    context.AddImage(imageRect, { .view = view,
                                  .sampler = sampler });
}

std::optional<Vector2> ViewportWidget::WindowToRenderTarget(const Vector2& windowPosition) const
{
    if (imageRect.size.x <= 0.0f || imageRect.size.y <= 0.0f)
        return std::nullopt;

    const Vector2 viewportPosition = Vector2(
        windowPosition.x - imageRect.position.x,
        windowPosition.y - imageRect.position.y);

    if (viewportPosition.x < 0.0f ||
        viewportPosition.y < 0.0f ||
        viewportPosition.x >= imageRect.size.x ||
        viewportPosition.y >= imageRect.size.y)
    {
        return std::nullopt;
    }

    if (targetExtent.width == 0 || targetExtent.height == 0)
        return std::nullopt;

    return Vector2(
        viewportPosition.x / imageRect.size.x * targetExtent.width,
        viewportPosition.y / imageRect.size.y * targetExtent.height);
}

bool ViewportWidget::SaveCurrScene()
{
    SceneSystem& sceneSystem = gEngine->GetSceneSystem();
    Scene* scene = sceneSystem.GetSceneByType(SceneType::Game);

    if (!scene)
        return false;

    const VirtualPath& scenePath = scene->GetFilePath();

    if (scenePath.ToString().empty())
    {
        URAY_LOG("Failed to save scene: Scene Path is empty.");
        return false;
    }

    VirtualFileSystem& filesystem = gEngine->GetAssetSystem().GetFileSystem();

    const std::string sceneText = YAML::Dump(scene->Serialize());

    if (!filesystem.WriteText(scenePath, sceneText))
    {
        URAY_LOG("Failed to save scene: %s", scenePath.ToString().c_str());
        return false;
    }

    URAY_LOG("Scene saved: %s", scenePath.ToString().c_str());
    return true;
}

} // namespace URay
