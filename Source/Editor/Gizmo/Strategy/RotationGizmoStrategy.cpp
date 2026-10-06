#include "RotationGizmoStrategy.h"

#include "Core/Math/Math.h"
#include "Editor/Gizmo/TransformGizmo.h"
#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"

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
    TransformComponent* transform = gizmo.GetTargetTransform();
    TransformComponent* cameraTransform = camera.GetOwner() ? camera.GetOwner()->GetTransform() : nullptr;
    if (!transform || !cameraTransform)
        return;
    selectedHandle = id;
    startWorldPosition = cameraTransform->GetPosition();
    startLineDirection = camera.ScreenToWorld(Vector3(position.x, position.y, 1.0f)) - startWorldPosition;
    targetInitPosition = transform->GetPosition();
    targetInitRotation = transform->GetRotation();

    if (gizmo.IsLocal())
    {
        switch (id)
        {
        case GizmoHandleId::X:
            axisDirection = transform->TransformVectorNoScale(Vector3::Right).GetNormalized();
            break;
        case GizmoHandleId::Y:
            axisDirection = transform->TransformVectorNoScale(Vector3::Forward).GetNormalized();
            break;
        case GizmoHandleId::Z:
            axisDirection = transform->TransformVectorNoScale(Vector3::Up).GetNormalized();
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
    TransformComponent* transform = gizmo.GetTargetTransform();
    if (!transform)
        return;

    Vector3 startHit, currentHit;
    if (!Math::IntersectLinePlane(startWorldPosition, startLineDirection, targetInitPosition, axisDirection, startHit) ||
        !Math::IntersectLinePlane(startWorldPosition, camera.ScreenToWorld(Vector3(position.x, position.y, 1.0f)) - startWorldPosition, targetInitPosition, axisDirection, currentHit))
        return;

    const Vector3 a = (startHit - targetInitPosition).GetNormalized();
    const Vector3 b = (currentHit - targetInitPosition).GetNormalized();
    if (a.GetSquaredLength() < Math::EPSILON || b.GetSquaredLength() < Math::EPSILON)
        return;

    const float angle = Math::RadToDeg(std::atan2(Vector3::Dot(axisDirection, Vector3::Cross(a, b)), Vector3::Dot(a, b)));

    const Matrix initial = Matrix::MakeRotation(targetInitRotation);
    const Matrix delta = GetAxisRotationMatrix(selectedHandle, angle);

    const Matrix result = gizmo.IsLocal() ? delta * initial : initial * delta;

    transform->SetRotation(MatrixToEulerXYZ(result));
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

Vector3 RotationGizmoStrategy::MatrixToEulerXYZ(const Matrix& matrix)
{
    constexpr float gimbalLockEpsilon = 1e-5f;

    const float sinY = std::clamp(-matrix.m02, -1.0f, 1.0f);
    const float y = std::asin(sinY);
    const float cosY = std::sqrt(std::max(0.0f, 1.0f - sinY * sinY));

    float x = 0.0f;
    float z = 0.0f;

    if (cosY > gimbalLockEpsilon)
    {
        x = std::atan2(matrix.m12, matrix.m22);
        z = std::atan2(matrix.m01, matrix.m00);
    }
    else
    {
        x = std::atan2(
            sinY >= 0.0f ? matrix.m10 : -matrix.m10,
            matrix.m11);
    }

    return Vector3(Math::RadToDeg(x), Math::RadToDeg(y), Math::RadToDeg(z));
}

} // namespace URay
