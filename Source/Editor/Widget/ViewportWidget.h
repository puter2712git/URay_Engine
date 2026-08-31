#pragma once

#include "Editor/Widget/Widget.h"

#include "Engine/Ray/EventRay.h"

#include "Core/Math/Extent2D.h"

#include <functional>
#include <memory>
#include <optional>

namespace URay
{

class CameraComponent;
class Engine;
class Unit;

class GizmoController;
class EditorPicker;
class SelectionSystem;

namespace Render
{
class Renderer;
}

class ViewportWidget : public Widget
{
public:
    ViewportWidget(Render::Renderer& renderer, CameraComponent& camera, Engine& engine, SelectionSystem& selectionSystem);
    ~ViewportWidget() override;

public:
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    EventReply OnKeyDown(const KeyEvent& event) override;
    EventReply OnKeyUp(const KeyEvent& event) override;

protected:
    void OnUpdate(float deltaTime) override;
    void OnPrepareRender(Render::DrawCommandBuilder& builder) override;
    void OnDraw() override;

private:
    void UpdateCameraMovement(float deltaTime);
    void UpdateCameraRotation();

    std::optional<Vector2> WindowToRenderTarget(const Vector2& windowPosition) const;

    bool SaveCurrScene();

private:
    Engine& engine;
    Render::Renderer& renderer;

    CameraComponent& camera;
    std::unique_ptr<GizmoController> gizmo = nullptr;
    std::unique_ptr<EditorPicker> picker = nullptr;

    SelectionSystem& selectionSystem;

    RayHandle onSelectedRayHandle;

    Rect imageRect = {};
    Extent2D targetExtent = {};

    Vector2 cachedPosition = Vector2::Zero;

    bool cameraForward = false;
    bool cameraBackward = false;
    bool cameraRight = false;
    bool cameraLeft = false;
    bool cameraUp = false;
    bool cameraDown = false;

    bool isCameraRotating = false;
    Vector2 pendingCameraLookDelta = Vector2::Zero;
};

} // namespace URay
