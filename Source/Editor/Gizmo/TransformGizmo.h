#pragma once

#include <Core/Math/Ray.h>
#include <Core/Math/Vector2.h>
#include <Editor/Gizmo/Strategy/GizmoStrategy.h>
#include <Editor/Viewport/EditorView.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay
{

class Unit;
class Material;
class TransformComponent;
struct Ray;

namespace Render
{
class GizmoObject;
}

class TransformGizmo
{
public:
    TransformGizmo();
    ~TransformGizmo();

public:
    void SetTargets(const std::vector<Unit*>& units, Unit* activeUnit);
    bool HasTargets() const { return !selectedUnits.empty(); }
    Unit* GetActiveUnit() const { return activeUnit; }
    TransformComponent* GetActiveTransform();
    const TransformComponent* GetActiveTransform() const;
    std::vector<TransformComponent*> GetTargetTransforms() const;
    Vector3 GetPivotPosition() const;

    void SetMode(GizmoMode newMode);
    GizmoMode GetMode() const;

    void SetHoveredHandle(GizmoHandleId handleId) { hoveredHandle = handleId; }
    bool IsDragging() const { return dragging; }

    bool IsLocal() const { return isLocal; }
    void SetIsLocal(bool local) { isLocal = local; }

    std::optional<GizmoHandleId> HitTest(const Ray& worldRay) const;

    void Update(const Ray& cursorRay, const EditorView& view);
    void StartDragging(GizmoHandleId handleId, const Ray& cursorRay, const EditorView& view);
    void UpdateDragging(const Ray& cursorRay);
    void EndDragging();

    const std::vector<GizmoHandle>& GetHandles() const;

private:
    GizmoStrategy* FindStrategy(GizmoMode mode) const;
    void UpdateRenderObject();

private:
    std::vector<std::unique_ptr<GizmoStrategy>> strategies;
    GizmoStrategy* currentStrategy = nullptr;

    bool dragging = false;

    GizmoHandleId hoveredHandle = GizmoHandleId::None;
    GizmoHandleId selectedHandle = GizmoHandleId::None;

    bool isLocal = false;

    std::vector<Unit*> selectedUnits;
    Unit* activeUnit = nullptr;
    Material* material = nullptr;
    Render::GizmoObject* renderObject = nullptr;

    const float scaleFactor = 0.1f;
    const float minScale = 0.25f;
    const float maxScale = 10.0f;
};

} // namespace URay
