#include "ScaleGizmoStrategy.h"

#include <Core/Math/Math.h>
#include <Editor/Gizmo/TransformGizmo.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Unit.h>

#include <algorithm>

namespace URay
{
ScaleGizmoStrategy::ScaleGizmoStrategy(TransformGizmo& gizmo) : gizmo(gizmo)
{
    AssetDatabase& assets = gEngine->GetAssetSystem().GetDatabase();
    Mesh* scaleMesh = assets.Find<Mesh>(EngineAsset::ScaleGizmoMesh);
    Mesh* quadMesh = assets.Find<Mesh>(EngineAsset::QuadMesh);
    Mesh* cubeMesh = assets.Find<Mesh>(EngineAsset::CubeMesh);
    handles = {
        { .id = GizmoHandleId::X, .mesh = scaleMesh, .color = Color::Red },
        { .id = GizmoHandleId::Y, .mesh = scaleMesh, .color = Color::Green },
        { .id = GizmoHandleId::Z, .mesh = scaleMesh, .color = Color::Blue },
        { .id = GizmoHandleId::XY, .mesh = quadMesh, .color = Color::Blue },
        { .id = GizmoHandleId::YZ, .mesh = quadMesh, .color = Color::Red },
        { .id = GizmoHandleId::XZ, .mesh = quadMesh, .color = Color::Green },
        { .id = GizmoHandleId::XYZ, .mesh = cubeMesh, .color = Color::White }
    };
}

void ScaleGizmoStrategy::UpdateHandles(const Matrix& world)
{
    handles[0].worldMatrix = Matrix::MakeRotationZ(-90.0f) * world;
    handles[1].worldMatrix = world;
    handles[2].worldMatrix = Matrix::MakeRotationX(90.0f) * world;
    const Matrix planeScale = Matrix::MakeScale(Vector3(0.35f, 0.35f, 0.35f));
    handles[3].worldMatrix = planeScale * Matrix::MakeTranslation(Vector3(0.45f, 0.0f, -0.45f)) * Matrix::MakeRotationX(90.0f) * world;
    handles[4].worldMatrix = planeScale * Matrix::MakeTranslation(Vector3(-0.45f, 0.0f, 0.45f)) * Matrix::MakeRotationZ(-90.0f) * world;
    handles[5].worldMatrix = planeScale * Matrix::MakeTranslation(Vector3(0.45f, 0.0f, 0.45f)) * world;
    handles[6].worldMatrix = Matrix::MakeScale(Vector3(0.18f, 0.18f, 0.18f)) * world;
}

void ScaleGizmoStrategy::StartDragging(const Vector2& position, GizmoHandleId id, const CameraComponent& camera)
{
    TransformComponent* transform = gizmo.GetTargetTransform();
    TransformComponent* cameraTransform = camera.GetOwner() ? camera.GetOwner()->GetTransform() : nullptr;
    if (!transform || !cameraTransform)
        return;
    selectedHandle = id;
    startWorldPosition = cameraTransform->GetWorldPosition();
    startLineDirection = camera.ScreenToWorld(Vector3(position.x, position.y, 1.0f)) - startWorldPosition;
    targetInitWorldPosition = transform->GetWorldPosition();
    targetInitScale = transform->GetLocalScale();

    xAxis = transform->GetWorldRight();
    yAxis = transform->GetWorldForward();
    zAxis = transform->GetWorldUp();

    switch (id)
    {
    case GizmoHandleId::X:
        planeNormal = Vector3::Cross(xAxis, Vector3::Cross(cameraTransform->GetWorldForward(), xAxis).GetNormalized()).GetNormalized();
        break;
    case GizmoHandleId::Y:
        planeNormal = Vector3::Cross(yAxis, Vector3::Cross(cameraTransform->GetWorldForward(), yAxis).GetNormalized()).GetNormalized();
        break;
    case GizmoHandleId::Z:
        planeNormal = Vector3::Cross(zAxis, Vector3::Cross(cameraTransform->GetWorldForward(), zAxis).GetNormalized()).GetNormalized();
        break;
    case GizmoHandleId::XY:
        planeNormal = zAxis;
        break;
    case GizmoHandleId::YZ:
        planeNormal = xAxis;
        break;
    case GizmoHandleId::XZ:
        planeNormal = yAxis;
        break;
    case GizmoHandleId::XYZ:
        planeNormal = cameraTransform->GetWorldForward();
        break;
    default:
        return;
    }
}

void ScaleGizmoStrategy::UpdateDragging(const Vector2& position, const CameraComponent& camera)
{
    TransformComponent* transform = gizmo.GetTargetTransform();
    if (!transform)
        return;
    Vector3 startHit, currentHit;
    if (!Math::IntersectLinePlane(startWorldPosition, startLineDirection, targetInitWorldPosition, planeNormal, startHit) ||
        !Math::IntersectLinePlane(startWorldPosition, camera.ScreenToWorld(Vector3(position.x, position.y, 1.0f)) - startWorldPosition, targetInitWorldPosition, planeNormal, currentHit))
        return;
    const Vector3 delta = currentHit - startHit;
    float amount = 0.0f;
    if (selectedHandle == GizmoHandleId::X)
        amount = Vector3::Dot(delta, xAxis);
    else if (selectedHandle == GizmoHandleId::Y)
        amount = Vector3::Dot(delta, yAxis);
    else if (selectedHandle == GizmoHandleId::Z)
        amount = Vector3::Dot(delta, zAxis);
    else
        amount = (Vector3::Dot(delta, xAxis) + Vector3::Dot(delta, yAxis) + Vector3::Dot(delta, zAxis)) / 3.0f;

    constexpr float minimumScale = 0.01f;
    Vector3 scale = targetInitScale;
    if (selectedHandle == GizmoHandleId::X || selectedHandle == GizmoHandleId::XY || selectedHandle == GizmoHandleId::XZ || selectedHandle == GizmoHandleId::XYZ)
        scale.x = std::max(minimumScale, targetInitScale.x + amount);
    if (selectedHandle == GizmoHandleId::Y || selectedHandle == GizmoHandleId::XY || selectedHandle == GizmoHandleId::YZ || selectedHandle == GizmoHandleId::XYZ)
        scale.y = std::max(minimumScale, targetInitScale.y + amount);
    if (selectedHandle == GizmoHandleId::Z || selectedHandle == GizmoHandleId::XZ || selectedHandle == GizmoHandleId::YZ || selectedHandle == GizmoHandleId::XYZ)
        scale.z = std::max(minimumScale, targetInitScale.z + amount);
    transform->SetLocalScale(scale);
}
} // namespace URay
