#pragma once

#include <Editor/Gizmo/Strategy/GizmoStrategy.h>

#include <Core/Math/Vector3.h>

#include <vector>

namespace URay
{
class TransformGizmo;
class TransformComponent;

class ScaleGizmoStrategy final : public GizmoStrategy
{
public:
    explicit ScaleGizmoStrategy(TransformGizmo& gizmo);
    GizmoMode GetMode() const override { return GizmoMode::Scale; }
    void UpdateHandles(const Matrix& gizmoWorld) override;
    void StartDragging(GizmoHandleId handleId, const Ray& cursorRay, const EditorView& view) override;
    void UpdateDragging(const Ray& cursorRay) override;
    void EndDragging() override;

private:
    struct TargetState
    {
        TransformComponent* transform = nullptr;
        Vector3 initialWorldPosition = Vector3::Zero;
        Vector3 initialLocalScale = Vector3::One;
    };

    TransformGizmo& gizmo;
    GizmoHandleId selectedHandle = GizmoHandleId::None;
    Vector3 startWorldPosition = Vector3::Zero;
    Vector3 startLineDirection = Vector3::Forward;
    Vector3 pivotPosition = Vector3::Zero;
    Vector3 activeInitScale = Vector3::One;
    Vector3 xAxis = Vector3::Right;
    Vector3 yAxis = Vector3::Forward;
    Vector3 zAxis = Vector3::Up;
    Vector3 planeNormal = Vector3::Up;
    std::vector<TargetState> targets;
};
} // namespace URay
