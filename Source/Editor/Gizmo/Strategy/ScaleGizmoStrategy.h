#pragma once

#include "Editor/Gizmo/Strategy/GizmoStrategy.h"

#include "Core/Math/Vector3.h"

namespace URay
{
class TransformGizmo;

class ScaleGizmoStrategy final : public GizmoStrategy
{
public:
    explicit ScaleGizmoStrategy(TransformGizmo& gizmo);
    GizmoMode GetMode() const override { return GizmoMode::Scale; }
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
    Vector3 targetInitScale = Vector3::One;
    Vector3 xAxis = Vector3::Right;
    Vector3 yAxis = Vector3::Forward;
    Vector3 zAxis = Vector3::Up;
    Vector3 planeNormal = Vector3::Up;
};
} // namespace URay
