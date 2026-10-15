#include "ViewportController.h"

#include <Core/Input/Action/InputActionSystem.h>
#include <Editor/EditorCamera.h>
#include <Editor/Gizmo/TransformGizmo.h>
#include <Editor/Input/EditorInputActions.h>
#include <Editor/Pick/PickSystem.h>
#include <Editor/Play/PlaySession.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Scene/Unit.h>
#include <Platform/Window/Window.h>
#include <Render/RHI/Texture/Sampler.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/ObjectIdReadback.h>
#include <Render/Rendering/RenderTargetRegistry.h>
#include <Render/Rendering/Renderer.h>
#include <Render/ResourceManager.h>

#include <cmath>

namespace URay
{

ViewportController::ViewportController(
    Render::Renderer& renderer,
    Render::ResourceManager& resourceManager,
    Window& window,
    InputActionSystem& inputActionSystem,
    SelectionSystem& selectionSystem,
    PlaySession& playSession,
    CameraComponent& editorCamera,
    bool& useEditorCamera)
    : renderer(renderer),
      resourceManager(resourceManager),
      window(window),
      inputActionSystem(inputActionSystem),
      selectionSystem(selectionSystem),
      playSession(playSession),
      useEditorCamera(useEditorCamera)
{
    gizmo = std::make_unique<TransformGizmo>();
    pickSystem = std::make_unique<PickSystem>(*gizmo);
    pickSystem->Initialize();

    this->editorCamera = std::make_unique<EditorCamera>();
    this->editorCamera->SetCamera(&editorCamera);

    selectionSystem.GetOnSelectionSetChangedRay().Register(
        this,
        [this]()
        {
            gizmo->SetTargets(
                this->selectionSystem.GetSelectedUnits(),
                this->selectionSystem.GetActiveUnit());
        });
    gizmo->SetTargets(selectionSystem.GetSelectedUnits(), selectionSystem.GetActiveUnit());

    renderer.GetObjectIdReadbackRay().Register(
        this,
        [this](const Render::ObjectIdReadbackResult& result)
        {
            if (!pendingSelectionRequest || pendingSelectionRequest->requestId != result.requestId)
                return;

            const SelectionOperation operation = pendingSelectionRequest->operation;
            pendingSelectionRequest.reset();

            Unit* unit = pickSystem->FindTarget(result.objectId);

            if (operation == SelectionOperation::Toggle)
            {
                if (unit)
                    this->selectionSystem.ToggleSelection(unit);

                return;
            }

            this->selectionSystem.SetSelection(unit);
        });
}

ViewportController::~ViewportController()
{
    renderer.GetObjectIdReadbackRay().UnregisterAll(this);
    selectionSystem.GetOnSelectionSetChangedRay().UnregisterAll(this);

    if (pickSystem)
    {
        pickSystem->Finalize();
        pickSystem.reset();
    }
}

void ViewportController::Update(float deltaTime, bool isFocused)
{
    CameraComponent* camera = editorCamera->GetCamera();
    if (!camera)
        return;

    PickResult pickResult = pickSystem->Pick(camera, cachedPosition.x, cachedPosition.y);
    gizmo->SetHoveredHandle(
        pickResult.isHit ? pickResult.gizmoHandle : GizmoHandleId::None);

    EditorCameraInput input = {};

    if (isFocused)
    {
        const InputActionSnapshot& actions = inputActionSystem.GetSnapshot();

        const auto wasPressed = [&actions](InputActionId id)
        {
            const InputActionValue* value = actions.FindAction(id);
            return value && value->pressed;
        };

        if (wasPressed(EditorInputAction::CycleGizmoMode))
            CycleGizmoMode();

        if (wasPressed(EditorInputAction::ToggleEditorCamera) && playSession.IsPlaying())
            useEditorCamera = !useEditorCamera;

        if (const InputActionValue* move = actions.FindAction(EditorInputAction::ViewportMove))
            input.horizontalAxis = Vector3(move->axis2D.x, move->axis2D.y, 0.0f);

        if (const InputActionValue* elevate = actions.FindAction(EditorInputAction::ViewportElevate))
            input.verticalAxis = Vector3(0.0f, 0.0f, elevate->axis1D);
    }

    gizmo->Update(cachedPosition, *camera);

    input.mouseDelta = cameraMouseDelta;
    editorCamera->Update(deltaTime, input);
    cameraMouseDelta = Vector2::Zero;
}

void ViewportController::SetDisplayRect(const Rect& rect)
{
    displayRect = rect;

    const Extent2D clientSize = window.GetClientSize();
    const Extent2D framebufferSize = window.GetFramebufferSize();

    const float scaleX = clientSize.width > 0
                             ? static_cast<float>(framebufferSize.width) / clientSize.width
                             : 1.0f;
    const float scaleY = clientSize.height > 0
                             ? static_cast<float>(framebufferSize.height) / clientSize.height
                             : 1.0f;

    renderer.RequestSceneRenderTargetResize({
        .width = static_cast<uint32>(std::round(displayRect.size.x * scaleX)),
        .height = static_cast<uint32>(std::round(displayRect.size.y * scaleY)),
    });
}

std::optional<ViewportImage> ViewportController::GetDisplayImage()
{
    Render::RenderTarget& postProcessRT = renderer.GetRenderTargetRegistry().Get(
        Render::RenderTargetId::PostProcess);

    Render::TextureView* view = postProcessRT.GetColorView(0);
    Render::Sampler* sampler = resourceManager.GetOrCreateSampler({});

    if (!view || !sampler)
        return std::nullopt;

    targetExtent = postProcessRT.GetExtent();

    if (CameraComponent* camera = editorCamera->GetCamera())
        camera->SetViewportExtent(targetExtent);

    return ViewportImage{ .view = view, .sampler = sampler };
}

EventReply ViewportController::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Right)
    {
        isCameraRotating = true;
        return { .requestFocus = true, .capturePointer = true };
    }

    if (event.changedButton == MouseButton::Left)
    {
        const auto targetPosition = WindowToRenderTarget(event.position);
        CameraComponent* camera = editorCamera->GetCamera();
        if (!targetPosition || !camera)
            return {};

        const PickResult pickResult = pickSystem->Pick(camera, targetPosition->x, targetPosition->y);
        if (pickResult.isHit && pickResult.gizmoHandle != GizmoHandleId::None)
        {
            gizmo->StartDragging(*targetPosition, pickResult.gizmoHandle, *camera);
        }
        else
        {
            const bool isToggleSelection = (event.modifiers & ModifierKey::Ctrl) != ModifierKey::None;
            const std::optional<uint64> requestId = renderer.GetObjectIdReadback().Request(
                static_cast<uint32>(targetPosition->x),
                static_cast<uint32>(targetPosition->y));

            if (requestId)
            {
                pendingSelectionRequest = PendingSelectionRequest{
                    .requestId = *requestId,
                    .operation = isToggleSelection ? SelectionOperation::Toggle : SelectionOperation::Replace,
                };
            }
        }
    }

    return { .requestFocus = true, .capturePointer = true };
}

EventReply ViewportController::OnPointerMove(const PointerEvent& event)
{
    if (isCameraRotating)
    {
        cameraMouseDelta.x += event.delta.x;
        cameraMouseDelta.y += event.delta.y;
        return {};
    }

    if (const auto targetPosition = WindowToRenderTarget(event.position))
        cachedPosition = *targetPosition;

    return {};
}

EventReply ViewportController::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Right)
        isCameraRotating = false;

    if (event.changedButton == MouseButton::Left && gizmo->IsDragging())
        gizmo->EndDragging();

    return { .releasePointer = event.pressedButtons == 0 };
}

void ViewportController::SetGizmoMode(GizmoMode mode)
{
    gizmo->SetMode(mode);
}

bool ViewportController::ToggleGizmoSpace()
{
    gizmo->SetIsLocal(!gizmo->IsLocal());
    return gizmo->IsLocal();
}

bool ViewportController::IsGizmoSpaceLocal() const
{
    return gizmo->IsLocal();
}

void ViewportController::Play()
{
    playSession.Start();
}

void ViewportController::Stop()
{
    playSession.Stop();
}

std::optional<Vector2> ViewportController::WindowToRenderTarget(const Vector2& windowPosition) const
{
    if (displayRect.size.x <= 0.0f || displayRect.size.y <= 0.0f ||
        targetExtent.width == 0 || targetExtent.height == 0)
    {
        return std::nullopt;
    }

    const Vector2 viewportPosition(
        windowPosition.x - displayRect.position.x,
        windowPosition.y - displayRect.position.y);

    if (viewportPosition.x < 0.0f || viewportPosition.y < 0.0f ||
        viewportPosition.x >= displayRect.size.x || viewportPosition.y >= displayRect.size.y)
    {
        return std::nullopt;
    }

    return Vector2(
        viewportPosition.x / displayRect.size.x * targetExtent.width,
        viewportPosition.y / displayRect.size.y * targetExtent.height);
}

void ViewportController::CycleGizmoMode()
{
    const int modeIndex = static_cast<int>(gizmo->GetMode());
    const GizmoMode newMode = static_cast<GizmoMode>(
        (modeIndex + 1) % static_cast<int>(GizmoMode::Count));
    gizmo->SetMode(newMode);
}

} // namespace URay
