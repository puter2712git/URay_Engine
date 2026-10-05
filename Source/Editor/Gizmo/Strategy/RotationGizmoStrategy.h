#pragma once

#include "Editor/Gizmo/Strategy/GizmoStrategy.h"

#include "Core/Math/Vector3.h"

namespace URay
{
class TransformGizmo;

class RotationGizmoStrategy final : public GizmoStrategy
{
public:
    explicit RotationGizmoStrategy(TransformGizmo& gizmo);
    GizmoMode GetMode() const override { return GizmoMode::Rotation; }
    void UpdateHandles(const Matrix& gizmoWorld) override;
    void StartDragging(const Vector2& position, GizmoHandleId handleId, const CameraComponent& camera) override;
    void UpdateDragging(const Vector2& position, const CameraComponent& camera) override;
    void EndDragging() override { selectedHandle = GizmoHandleId::None; }

private:
    TransformGizmo& gizmo;
    GizmoHandleId selectedHandle = GizmoHandleId::None;
    Vector3 startWorldPosition = Vector3::Zero;
    Vector3 startLineDirection = Vector3::Forward;
    Vector3 targetInitPosition = Vector3::Zero;
    Vector3 targetInitRotation = Vector3::Zero;
    Vector3 axisDirection = Vector3::Forward;
};
} // namespace URay
