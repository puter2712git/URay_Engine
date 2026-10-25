#pragma once

#include <Core/Math/Vector3.h>
#include <Editor/Gizmo/Strategy/GizmoStrategy.h>

#include <vector>

namespace URay
{

class TransformGizmo;
class TransformComponent;

class TranslationGizmoStrategy final : public GizmoStrategy
{
public:
    TranslationGizmoStrategy(TransformGizmo& gizmo);
    ~TranslationGizmoStrategy() override;

public:
    GizmoMode GetMode() const override { return GizmoMode::Translation; }
    void UpdateHandles(const Matrix& gizmoWorld) override;
    void StartDragging(GizmoHandleId handleId, const Vector3& origin, const Vector3& direction, const EditorView& view) override;
    void UpdateDragging(const Vector3& origin, const Vector3& direction) override;
    void EndDragging() override;

private:
    struct TargetState
    {
        TransformComponent* transform = nullptr;
        Vector3 initialWorldPosition = Vector3::Zero;
    };

    TransformGizmo& gizmo;

    Vector3 startWorldPosition = Vector3::Zero;
    Vector3 startLineDirection = Vector3::Forward;
    Vector3 pivotPosition = Vector3::Zero;
    std::vector<TargetState> targets;
    GizmoHandleId selectedHandle = GizmoHandleId::None;
    Vector3 xAxis = Vector3::Right;
    Vector3 yAxis = Vector3::Forward;
    Vector3 zAxis = Vector3::Up;
    Vector3 planeNormal = Vector3::Up;
};

} // namespace URay
