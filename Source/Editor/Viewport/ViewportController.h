#pragma once

#include <Core/Input/InputEvent.h>
#include <Core/Math/Extent2D.h>
#include <Core/Math/Rect.h>
#include <Core/Math/Vector2.h>
#include <Core/Type/Ray.h>
#include <Core/Type/Types.h>
#include <Editor/Gizmo/Strategy/GizmoStrategy.h>
#include <Editor/Viewport/EditorView.h>
#include <Engine/UI/Input/EventReply.h>
#include <Render/Rendering/View/ViewMode.h>

#include <memory>
#include <optional>

namespace URay
{

class EditorCamera;
class ObjectIdTargetRegistry;
class PlaySession;
class SelectionSystem;
class TransformGizmo;

class InputActionSystem;
class Window;

namespace Render
{
class Renderer;
class ResourceManager;
class Sampler;
class SceneSystem;
class TextureView;
} // namespace Render

struct ViewportImage
{
    Render::TextureView* view = nullptr;
    Render::Sampler* sampler = nullptr;
};

class ViewportController
{
public:
    ViewportController(
        Render::Renderer& renderer,
        Render::ResourceManager& resourceManager,
        Render::SceneSystem& renderSceneSystem,
        Window& window,
        InputActionSystem& inputActionSystem,
        SelectionSystem& selectionSystem,
        PlaySession& playSession,
        bool& useEditorCamera);
    ~ViewportController();

public:
    void Update(float deltaTime, bool isFocused);

    void SetDisplayRect(const Rect& rect);
    std::optional<ViewportImage> GetDisplayImage();

    EventReply OnPointerDown(const PointerEvent& event);
    EventReply OnPointerMove(const PointerEvent& event);
    EventReply OnPointerUp(const PointerEvent& event);

    void SetGizmoMode(GizmoMode mode);
    bool ToggleGizmoSpace();
    bool IsGizmoSpaceLocal() const;
    void CycleViewMode();
    void Play();
    void Stop();

    const Extent2D& GetTargetExtent() const { return targetExtent; }
    Render::ViewMode GetViewMode() const { return viewMode; }

    EditorCamera& GetEditorCamera() { return *editorCamera; }

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
    std::optional<GizmoHandleId> HitTestGizmo(const EditorView& view, const Vector2& targetPosition) const;
    void CycleGizmoMode();

    void FocusActiveUnit();

private:
    Render::Renderer& renderer;
    Render::ResourceManager& resourceManager;
    Window& window;
    InputActionSystem& inputActionSystem;
    SelectionSystem& selectionSystem;
    PlaySession& playSession;
    bool& useEditorCamera;

    std::unique_ptr<EditorCamera> editorCamera = nullptr;
    std::unique_ptr<TransformGizmo> gizmo = nullptr;
    std::unique_ptr<ObjectIdTargetRegistry> objectIdTargets = nullptr;

    bool isCameraRotating = false;
    Vector2 cameraMouseDelta = Vector2::Zero;
    Vector2 cachedPosition = Vector2::Zero;

    Rect displayRect = {};
    Extent2D targetExtent = {};
    std::optional<PendingSelectionRequest> pendingSelectionRequest = std::nullopt;
    Render::ViewMode viewMode = Render::ViewMode::Lit;
};

} // namespace URay
