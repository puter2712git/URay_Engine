#include "ViewportController.h"

#include <Core/Math/Math.h>

#include <Core/Input/Action/InputActionSystem.h>
#include <Core/Math/Ray.h>
#include <Editor/EditorCamera.h>
#include <Editor/Gizmo/TransformGizmo.h>
#include <Editor/Input/EditorInputActions.h>
#include <Editor/Pick/ObjectIdTargetRegistry.h>
#include <Editor/Play/PlaySession.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Scene/Unit.h>
#include <Platform/Window/Window.h>
#include <Render/RHI/RenderTarget.h>
#include <Render/RHI/Texture/Sampler.h>
#include <Render/Rendering/ObjectIdReadback.h>
#include <Render/Rendering/Renderer.h>
#include <Render/Rendering/Scene/SceneSystem.h>
#include <Render/ResourceManager.h>

#include <cmath>

namespace URay
{

ViewportController::ViewportController(
    Render::Renderer& renderer,
    Render::ResourceManager& resourceManager,
    Render::SceneSystem& renderSceneSystem,
    Window& window,
    InputActionSystem& inputActionSystem,
    SelectionSystem& selectionSystem,
    PlaySession& playSession,
    bool& useEditorCamera)
    : renderer(renderer),
      resourceManager(resourceManager),
      window(window),
      inputActionSystem(inputActionSystem),
      selectionSystem(selectionSystem),
      playSession(playSession),
      useEditorCamera(useEditorCamera)
{
    gizmo = std::make_unique<TransformGizmo>(playSession);
    objectIdTargets = std::make_unique<ObjectIdTargetRegistry>(renderSceneSystem);

    this->editorCamera = std::make_unique<EditorCamera>();

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

            Unit* unit = objectIdTargets->FindUnit(result.objectId);

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

    objectIdTargets.reset();
}

void ViewportController::Update(float deltaTime, bool isFocused)
{
    gizmo->SetHoveredHandle(
        HitTestGizmo(editorCamera->GetView(), cachedPosition).value_or(GizmoHandleId::None));

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

        if (wasPressed(EditorInputAction::FocusActiveUnit))
            FocusActiveUnit();
    }

    const Ray cursorRay = editorCamera->GetView().ScreenPointToRay(cachedPosition, targetExtent);

    gizmo->Update(cursorRay, editorCamera->GetView());

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
        .width = static_cast<uint32>(Math::Round(displayRect.size.x * scaleX)),
        .height = static_cast<uint32>(Math::Round(displayRect.size.y * scaleY)),
    });
}

std::optional<ViewportImage> ViewportController::GetDisplayImage()
{
    const Render::SceneRenderTargets targets = renderer.GetViewportSceneRenderTargets();
    if (!targets.postProcess)
        return std::nullopt;

    Render::RenderTarget& postProcessRT = *targets.postProcess;

    Render::TextureView* view = postProcessRT.GetColorView(0);
    Render::Sampler* sampler = resourceManager.GetOrCreateSampler({});

    if (!view || !sampler)
        return std::nullopt;

    targetExtent = postProcessRT.GetExtent();

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
        const EditorView& view = editorCamera->GetView();
        if (!targetPosition)
            return {};

        const Ray cursorRay = view.ScreenPointToRay(*targetPosition, targetExtent);

        if (const std::optional<GizmoHandleId> gizmoHandle = HitTestGizmo(view, *targetPosition))
        {
            gizmo->StartDragging(*gizmoHandle, cursorRay, editorCamera->GetView());
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

void ViewportController::CycleViewMode()
{
    const uint8 nextMode =
        (static_cast<uint8>(viewMode) + 1u) % static_cast<uint8>(Render::ViewMode::Count);

    viewMode = static_cast<Render::ViewMode>(nextMode);
}

void ViewportController::Play()
{
    playSession.Start();
}

void ViewportController::Stop()
{
    gizmo->Deactivate();
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

std::optional<GizmoHandleId> ViewportController::HitTestGizmo(
    const EditorView& view,
    const Vector2& targetPosition) const
{
    const Ray screenRay = view.ScreenPointToRay(targetPosition, targetExtent);

    return gizmo->HitTest(screenRay);
}

void ViewportController::CycleGizmoMode()
{
    const int modeIndex = static_cast<int>(gizmo->GetMode());
    const GizmoMode newMode = static_cast<GizmoMode>(
        (modeIndex + 1) % static_cast<int>(GizmoMode::Count));
    gizmo->SetMode(newMode);
}

void ViewportController::FocusActiveUnit()
{
    Unit* activeUnit = selectionSystem.GetActiveUnit();
    if (!activeUnit)
        return;

    TransformComponent* transform = activeUnit->GetTransform();
    if (!transform)
        return;

    constexpr float FocusDistance = 5.0f;
    editorCamera->FocusAt(transform->GetWorldPosition(), FocusDistance);
}

} // namespace URay
