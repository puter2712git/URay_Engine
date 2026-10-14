#pragma once

#include <Core/Math/Extent2D.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Ray/EventRay.h>
#include <Core/Type/Types.h>
#include <Editor/Widget/Panel/Panel.h>
#include <Render/Rendering/View/ViewMode.h>

#include <functional>
#include <memory>
#include <optional>

namespace URay
{

class CameraComponent;
class Engine;

class TransformGizmo;
class PickSystem;
class SelectionSystem;
class Editor;
class HorizontalLayout;
class EditorCamera;
class ViewportToolbar;

namespace Render
{
class Renderer;
}

class ViewportPanel : public Panel
{
    URAY_TYPE(ViewportPanel, Panel)

public:
    ViewportPanel();
    ~ViewportPanel() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    void SetCamera(CameraComponent* camera);

    const Extent2D& GetTargetExtent() const { return targetExtent; }

    Render::ViewMode GetViewMode() const { return viewMode; }

protected:
    void OnUpdate(float deltaTime) override;
    void OnPaint(Render::UIDrawContext& context) override;

private:
    enum class SelectionOperation
    {
        Replace,
        Toggle
    };

    struct PendingSelectionRequest
    {
        uint64 requestId = 0;
        SelectionOperation operation = SelectionOperation::Replace;
    };

private:
    std::optional<Vector2> WindowToRenderTarget(const Vector2& windowPosition) const;

    void CycleGizmoMode();

private:
    Render::Renderer& renderer;

    ViewportToolbar* toolbar = nullptr;

    std::unique_ptr<EditorCamera> editorCamera = nullptr;

    bool isCameraRotating = false;
    Vector2 cameraMouseDelta = Vector2::Zero;

    std::unique_ptr<TransformGizmo> gizmo = nullptr;
    std::unique_ptr<PickSystem> pickSystem = nullptr;

    SelectionSystem& selectionSystem;
    std::optional<PendingSelectionRequest> pendingSelectionRequest = std::nullopt;

    Rect imageRect = {};
    Extent2D targetExtent = {};

    Vector2 cachedPosition = Vector2::Zero;

    bool isPerspective = true;

    Render::ViewMode viewMode = Render::ViewMode::Lit;

    HorizontalLayout* layout = nullptr;
};

} // namespace URay
