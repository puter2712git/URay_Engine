#include "RotationGizmoStrategy.h"

#include <Core/Math/Math.h>
#include <Editor/Gizmo/TransformGizmo.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Unit.h>

namespace URay
{

RotationGizmoStrategy::RotationGizmoStrategy(TransformGizmo& gizmo) : gizmo(gizmo)
{
    Mesh* mesh = gEngine->GetAssetSystem().GetDatabase().Find<Mesh>(EngineAsset::RotationGizmoMesh);
    handles = { { .id = GizmoHandleId::X, .mesh = mesh, .color = Color::Red }, { .id = GizmoHandleId::Y, .mesh = mesh, .color = Color::Green }, { .id = GizmoHandleId::Z, .mesh = mesh, .color = Color::Blue } };
}

void RotationGizmoStrategy::UpdateHandles(const Matrix& world)
{
    handles[0].worldMatrix = Matrix::MakeRotationY(90.0f) * world;
    handles[1].worldMatrix = Matrix::MakeRotationX(90.0f) * world;
    handles[2].worldMatrix = world;
}

void RotationGizmoStrategy::StartDragging(GizmoHandleId id, const Vector3& origin, const Vector3& direction, const EditorView&)
{
    TransformComponent* transform = gizmo.GetActiveTransform();
    if (!transform)
        return;
    selectedHandle = id;
    startWorldPosition = origin;
    startLineDirection = direction;
    targets.clear();
    for (TransformComponent* target : gizmo.GetTargetTransforms())
    {
        targets.push_back({ .transform = target,
                            .initialWorldPosition = target->GetWorldPosition(),
                            .initialLocalRotation = target->GetLocalRotation(),
                            .initialWorldRotation = target->GetWorldRotation() });
    }
    if (targets.empty())
        return;

    pivotPosition = gizmo.GetPivotPosition();
    activeInitWorldRotation = transform->GetWorldRotation();

    if (gizmo.IsLocal())
    {
        switch (id)
        {
        case GizmoHandleId::X:
            axisDirection = transform->GetWorldRight();
            break;
        case GizmoHandleId::Y:
            axisDirection = transform->GetWorldForward();
            break;
        case GizmoHandleId::Z:
            axisDirection = transform->GetWorldUp();
            break;
        }
    }
    else
    {
        switch (id)
        {
        case GizmoHandleId::X:
            axisDirection = Vector3::Right;
            break;
        case GizmoHandleId::Y:
            axisDirection = Vector3::Forward;
            break;
        case GizmoHandleId::Z:
            axisDirection = Vector3::Up;
            break;
        }
    }
}

void RotationGizmoStrategy::UpdateDragging(const Vector3& origin, const Vector3& direction)
{
    if (targets.empty())
        return;

    Vector3 startHit, currentHit;
    if (!Math::IntersectLinePlane(startWorldPosition, startLineDirection, pivotPosition, axisDirection, startHit) ||
        !Math::IntersectLinePlane(origin, direction, pivotPosition, axisDirection, currentHit))
        return;

    const Vector3 a = (startHit - pivotPosition).GetNormalized();
    const Vector3 b = (currentHit - pivotPosition).GetNormalized();
    if (a.GetSquaredLength() < Math::EPSILON || b.GetSquaredLength() < Math::EPSILON)
        return;

    const float angle = Math::RadToDeg(Math::Atan2(Vector3::Dot(axisDirection, Vector3::Cross(a, b)), Vector3::Dot(a, b)));

    const Matrix delta = GetAxisRotationMatrix(selectedHandle, angle);
    Matrix worldDelta = delta;
    if (gizmo.IsLocal())
    {
        const Matrix activeRotation = Matrix::MakeRotation(activeInitWorldRotation);
        worldDelta = activeRotation.Inverse() * delta * activeRotation;
    }

    for (const TargetState& target : targets)
    {
        const Vector3 rotatedOffset = worldDelta.TransformVector(target.initialWorldPosition - pivotPosition);
        target.transform->SetWorldPosition(pivotPosition + rotatedOffset);

        if (gizmo.IsLocal())
        {
            const Matrix initialLocal = Matrix::MakeRotation(target.initialLocalRotation);
            target.transform->SetLocalRotation((delta * initialLocal).GetRotation());
        }
        else
        {
            const Matrix initialWorld = Matrix::MakeRotation(target.initialWorldRotation);
            target.transform->SetWorldRotation((initialWorld * delta).GetRotation());
        }
    }
}

void RotationGizmoStrategy::EndDragging()
{
    selectedHandle = GizmoHandleId::None;
    targets.clear();
}

Matrix RotationGizmoStrategy::GetAxisRotationMatrix(GizmoHandleId handleId, float angle)
{
    switch (handleId)
    {
    case GizmoHandleId::X:
        return Matrix::MakeRotationX(angle);
    case GizmoHandleId::Y:
        return Matrix::MakeRotationY(angle);
    case GizmoHandleId::Z:
        return Matrix::MakeRotationZ(angle);
    default:
        return Matrix::Identity;
    }
}

} // namespace URay
