#include "MeshPreviewPanel.h"

#include <Core/Input/InputSystem.h>
#include <Editor/Asset/Mesh/MeshPreviewSession.h>
#include <Engine/Engine.h>
#include <Engine/UI/Widget/WindowWidget.h>
#include <Platform/Window/WindowSystem.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

#include <cassert>
#include <utility>

namespace URay
{

MeshPreviewPanel::MeshPreviewPanel(std::unique_ptr<MeshPreviewSession> session)
    : session(std::move(session))
{
    assert(this->session);
}

MeshPreviewPanel::~MeshPreviewPanel() = default;

const InputSnapshot* MeshPreviewPanel::FindWindowInputSnapshot() const
{
    Widget* current = GetParent();
    while (current)
    {
        if (WindowWidget* windowWidget = Cast<WindowWidget>(current))
        {
            Window& window = windowWidget->GetWindow();
            InputSystem& inputSystem = gEngine->GetInputSystem();

            if (&window == &gEngine->GetWindowSystem().GetMainWindow())
                return &inputSystem.GetSnapshot();

            WindowInputState* state = inputSystem.FindWindowState(window);
            return state ? &state->GetSnapshot() : nullptr;
        }

        current = current->GetParent();
    }

    return nullptr;
}

EventReply MeshPreviewPanel::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Right)
        return {};

    isCameraRotating = true;
    return {
        .handled = true,
        .requestFocus = true,
        .capturePointer = true
    };
}

EventReply MeshPreviewPanel::OnPointerMove(const PointerEvent& event)
{
    if (!isCameraRotating)
        return {};

    cameraMouseDelta += event.delta;
    return { .handled = true };
}

EventReply MeshPreviewPanel::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Right || !isCameraRotating)
        return {};

    isCameraRotating = false;
    return {
        .handled = true,
        .releasePointer = true
    };
}

void MeshPreviewPanel::OnPointerCancel()
{
    isCameraRotating = false;
    cameraMouseDelta = Vector2::Zero;
}

void MeshPreviewPanel::OnUpdate(float deltaTime)
{
    EditorCameraInput cameraInput = {};
    const InputSnapshot* input = FindWindowInputSnapshot();

    if (IsVisible() && IsFocused() && input && input->IsWindowFocused())
    {
        const auto held = [input](KeyCode key)
        {
            return input->IsKeyHeld(key) ? 1.0f : 0.0f;
        };

        cameraInput.horizontalAxis = Vector3(
            held(KeyCode::D) - held(KeyCode::A),
            held(KeyCode::W) - held(KeyCode::S),
            0.0f);
        cameraInput.verticalAxis = Vector3(
            0.0f,
            0.0f,
            held(KeyCode::E) - held(KeyCode::Q));
        cameraInput.mouseDelta = cameraMouseDelta;
    }

    session->GetCamera().Update(deltaTime, cameraInput);
    cameraMouseDelta = Vector2::Zero;
}

void MeshPreviewPanel::OnPaint(Render::UIDrawContext& context)
{
    Panel::OnPaint(context);

    const Rect imageRect = GetContentRect();
    if (imageRect.size.x <= 0.0f || imageRect.size.y <= 0.0f)
        return;

    Render::TextureView* view = session->GetDisplayView();
    if (!view)
        return;

    context.AddImage(imageRect, { .view = view });
}

} // namespace URay
