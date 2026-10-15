#include "RotationGizmoStrategy.h"

#include <Core/Math/Math.h>
#include <Editor/Gizmo/TransformGizmo.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Component/Render/CameraComponent.h>
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

void RotationGizmoStrategy::StartDragging(const Vector2& position, GizmoHandleId id, const CameraComponent& camera)
{
    TransformComponent* transform = gizmo.GetActiveTransform();
    TransformComponent* cameraTransform = camera.GetOwner() ? camera.GetOwner()->GetTransform() : nullptr;
    if (!transform || !cameraTransform)
        return;
    selectedHandle = id;
    startWorldPosition = cameraTransform->GetWorldPosition();
    startLineDirection = camera.ScreenToWorld(Vector3(position.x, position.y, 1.0f)) - startWorldPosition;
    targets.clear();
    for (TransformComponent* target : gizmo.GetTargetTransforms())
    {
        targets.push_back({
            .transform = target,
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

void RotationGizmoStrategy::UpdateDragging(const Vector2& position, const CameraComponent& camera)
{
    if (targets.empty())
        return;

    Vector3 startHit, currentHit;
    if (!Math::IntersectLinePlane(startWorldPosition, startLineDirection, pivotPosition, axisDirection, startHit) ||
        !Math::IntersectLinePlane(startWorldPosition, camera.ScreenToWorld(Vector3(position.x, position.y, 1.0f)) - startWorldPosition, pivotPosition, axisDirection, currentHit))
        return;

    const Vector3 a = (startHit - pivotPosition).GetNormalized();
    const Vector3 b = (currentHit - pivotPosition).GetNormalized();
    if (a.GetSquaredLength() < Math::EPSILON || b.GetSquaredLength() < Math::EPSILON)
        return;

    const float angle = Math::RadToDeg(std::atan2(Vector3::Dot(axisDirection, Vector3::Cross(a, b)), Vector3::Dot(a, b)));

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
