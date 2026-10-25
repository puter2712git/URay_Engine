#include "ScaleGizmoStrategy.h"

#include <Core/Math/Math.h>
#include <Editor/Gizmo/TransformGizmo.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
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

void ScaleGizmoStrategy::StartDragging(GizmoHandleId id, const Vector3& origin, const Vector3& direction, const EditorView& view)
{
    TransformComponent* transform = gizmo.GetActiveTransform();
    if (!transform)
        return;
    selectedHandle = id;
    startWorldPosition = origin;
    startLineDirection = direction;
    targets.clear();
    for (TransformComponent* target : gizmo.GetTargetTransforms())
        targets.push_back({ .transform = target, .initialWorldPosition = target->GetWorldPosition(), .initialLocalScale = target->GetLocalScale() });
    if (targets.empty())
        return;

    pivotPosition = gizmo.GetPivotPosition();
    activeInitScale = transform->GetLocalScale();

    xAxis = transform->GetWorldRight();
    yAxis = transform->GetWorldForward();
    zAxis = transform->GetWorldUp();

    const Vector3 cameraForward = view.GetForward();

    switch (id)
    {
    case GizmoHandleId::X:
        planeNormal = Vector3::Cross(xAxis, Vector3::Cross(cameraForward, xAxis).GetNormalized()).GetNormalized();
        break;
    case GizmoHandleId::Y:
        planeNormal = Vector3::Cross(yAxis, Vector3::Cross(cameraForward, yAxis).GetNormalized()).GetNormalized();
        break;
    case GizmoHandleId::Z:
        planeNormal = Vector3::Cross(zAxis, Vector3::Cross(cameraForward, zAxis).GetNormalized()).GetNormalized();
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
        planeNormal = cameraForward;
        break;
    default:
        return;
    }
}

void ScaleGizmoStrategy::UpdateDragging(const Vector3& origin, const Vector3& direction)
{
    if (targets.empty())
        return;
    Vector3 startHit, currentHit;
    if (!Math::IntersectLinePlane(startWorldPosition, startLineDirection, pivotPosition, planeNormal, startHit) ||
        !Math::IntersectLinePlane(origin, direction, pivotPosition, planeNormal, currentHit))
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
    Vector3 activeScale = activeInitScale;
    if (selectedHandle == GizmoHandleId::X || selectedHandle == GizmoHandleId::XY || selectedHandle == GizmoHandleId::XZ || selectedHandle == GizmoHandleId::XYZ)
        activeScale.x = Math::Max(minimumScale, activeInitScale.x + amount);
    if (selectedHandle == GizmoHandleId::Y || selectedHandle == GizmoHandleId::XY || selectedHandle == GizmoHandleId::YZ || selectedHandle == GizmoHandleId::XYZ)
        activeScale.y = Math::Max(minimumScale, activeInitScale.y + amount);
    if (selectedHandle == GizmoHandleId::Z || selectedHandle == GizmoHandleId::XZ || selectedHandle == GizmoHandleId::YZ || selectedHandle == GizmoHandleId::XYZ)
        activeScale.z = Math::Max(minimumScale, activeInitScale.z + amount);

    const Vector3 factors(
        activeInitScale.x != 0.0f ? activeScale.x / activeInitScale.x : 1.0f,
        activeInitScale.y != 0.0f ? activeScale.y / activeInitScale.y : 1.0f,
        activeInitScale.z != 0.0f ? activeScale.z / activeInitScale.z : 1.0f);

    for (const TargetState& target : targets)
    {
        const Vector3 offset = target.initialWorldPosition - pivotPosition;
        const Vector3 scaledOffset =
            xAxis * Vector3::Dot(offset, xAxis) * factors.x +
            yAxis * Vector3::Dot(offset, yAxis) * factors.y +
            zAxis * Vector3::Dot(offset, zAxis) * factors.z;

        target.transform->SetWorldPosition(pivotPosition + scaledOffset);
        target.transform->SetLocalScale(Vector3(
            Math::Max(minimumScale, target.initialLocalScale.x * factors.x),
            Math::Max(minimumScale, target.initialLocalScale.y * factors.y),
            Math::Max(minimumScale, target.initialLocalScale.z * factors.z)));
    }
}

void ScaleGizmoStrategy::EndDragging()
{
    selectedHandle = GizmoHandleId::None;
    targets.clear();
}
} // namespace URay
