#pragma once

#include <Editor/Gizmo/Strategy/GizmoStrategy.h>

#include <Core/Math/Vector3.h>

#include <vector>

namespace URay
{
class TransformGizmo;
class TransformComponent;

class RotationGizmoStrategy final : public GizmoStrategy
{
public:
    explicit RotationGizmoStrategy(TransformGizmo& gizmo);
    GizmoMode GetMode() const override { return GizmoMode::Rotation; }
    void UpdateHandles(const Matrix& gizmoWorld) override;
    void StartDragging(GizmoHandleId handleId, const Ray& cursorRay, const EditorView& view) override;
    void UpdateDragging(const Ray& cursorRay) override;
    void EndDragging() override;

private:
    struct TargetState
    {
        TransformComponent* transform = nullptr;
        Vector3 initialWorldPosition = Vector3::Zero;
        Vector3 initialLocalRotation = Vector3::Zero;
        Vector3 initialWorldRotation = Vector3::Zero;
    };

    Matrix GetAxisRotationMatrix(GizmoHandleId selectedHandle, float angle);

private:
    TransformGizmo& gizmo;
    GizmoHandleId selectedHandle = GizmoHandleId::None;
    Vector3 startWorldPosition = Vector3::Zero;
    Vector3 startLineDirection = Vector3::Forward;
    Vector3 pivotPosition = Vector3::Zero;
    Vector3 activeInitWorldRotation = Vector3::Zero;
    Vector3 axisDirection = Vector3::Forward;
    std::vector<TargetState> targets;
};
} // namespace URay
