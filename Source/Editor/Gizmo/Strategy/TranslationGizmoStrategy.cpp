#include "TranslationGizmoStrategy.h"

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

TranslationGizmoStrategy::TranslationGizmoStrategy(TransformGizmo& gizmo)
    : gizmo(gizmo)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    Mesh* arrowMesh = assetDatabase.Find<Mesh>(EngineAsset::ArrowMesh);
    Mesh* quadMesh = assetDatabase.Find<Mesh>(EngineAsset::QuadMesh);
    Mesh* cubeMesh = assetDatabase.Find<Mesh>(EngineAsset::CubeMesh);

    handles.push_back(GizmoHandle{
        .id = GizmoHandleId::X,
        .mesh = arrowMesh,
        .worldMatrix = Matrix::MakeRotationZ(-90.0f),
        .color = Color::Red });
    handles.push_back(GizmoHandle{
        .id = GizmoHandleId::Y,
        .mesh = arrowMesh,
        .worldMatrix = Matrix::Identity,
        .color = Color::Green });
    handles.push_back(GizmoHandle{
        .id = GizmoHandleId::Z,
        .mesh = arrowMesh,
        .worldMatrix = Matrix::MakeRotationX(90.0f),
        .color = Color::Blue });
    handles.push_back(GizmoHandle{ .id = GizmoHandleId::XY, .mesh = quadMesh, .color = Color::Blue });
    handles.push_back(GizmoHandle{ .id = GizmoHandleId::YZ, .mesh = quadMesh, .color = Color::Red });
    handles.push_back(GizmoHandle{ .id = GizmoHandleId::XZ, .mesh = quadMesh, .color = Color::Green });
    handles.push_back(GizmoHandle{ .id = GizmoHandleId::XYZ, .mesh = cubeMesh, .color = Color::White });
}

TranslationGizmoStrategy::~TranslationGizmoStrategy() = default;

void TranslationGizmoStrategy::UpdateHandles(const Matrix& gizmoWorld)
{
    handles[0].worldMatrix = Matrix::MakeRotationZ(-90.0f) * gizmoWorld;
    handles[1].worldMatrix = gizmoWorld;
    handles[2].worldMatrix = Matrix::MakeRotationX(90.0f) * gizmoWorld;

    const Matrix planeScale = Matrix::MakeScale(Vector3(0.35f, 0.35f, 0.35f));
    handles[3].worldMatrix = planeScale * Matrix::MakeTranslation(Vector3(0.45f, 0.0f, -0.45f)) * Matrix::MakeRotationX(90.0f) * gizmoWorld;
    handles[4].worldMatrix = planeScale * Matrix::MakeTranslation(Vector3(-0.45f, 0.0f, 0.45f)) * Matrix::MakeRotationZ(-90.0f) * gizmoWorld;
    handles[5].worldMatrix = planeScale * Matrix::MakeTranslation(Vector3(0.45f, 0.0f, 0.45f)) * gizmoWorld;
    handles[6].worldMatrix = Matrix::MakeScale(Vector3(0.18f, 0.18f, 0.18f)) * gizmoWorld;
}

void TranslationGizmoStrategy::StartDragging(
    GizmoHandleId handleId,
    const Ray& cursorRay,
    const EditorView& view)
{
    TransformComponent* activeTransform = gizmo.GetActiveTransform();
    if (!activeTransform)
        return;

    startWorldPosition = cursorRay.origin;
    startLineDirection = cursorRay.direction;
    targets.clear();
    for (TransformComponent* transform : gizmo.GetTargetTransforms())
        targets.push_back({ .transform = transform, .initialWorldPosition = transform->GetWorldPosition() });
    if (targets.empty())
        return;

    pivotPosition = gizmo.GetPivotPosition();
    selectedHandle = handleId;

    if (gizmo.IsLocal())
    {
        xAxis = activeTransform->GetWorldRight();
        yAxis = activeTransform->GetWorldForward();
        zAxis = activeTransform->GetWorldUp();
    }
    else
    {
        xAxis = Vector3::Right;
        yAxis = Vector3::Forward;
        zAxis = Vector3::Up;
    }

    const Vector3 cameraForward = view.GetForward().GetNormalized();

    switch (selectedHandle)
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

void TranslationGizmoStrategy::UpdateDragging(
    const Ray& cursorRay)
{
    if (targets.empty())
        return;

    Vector3 startHit;
    Vector3 currentHit;
    if (!Math::IntersectLinePlane(startWorldPosition, startLineDirection, pivotPosition, planeNormal, startHit) ||
        !Math::IntersectLinePlane(cursorRay.origin, cursorRay.direction, pivotPosition, planeNormal, currentHit))
        return;

    const Vector3 delta = currentHit - startHit;
    Vector3 move = Vector3::Zero;
    switch (selectedHandle)
    {
    case GizmoHandleId::X:
        move = xAxis * Vector3::Dot(delta, xAxis);
        break;
    case GizmoHandleId::Y:
        move = yAxis * Vector3::Dot(delta, yAxis);
        break;
    case GizmoHandleId::Z:
        move = zAxis * Vector3::Dot(delta, zAxis);
        break;
    case GizmoHandleId::XY:
        move = xAxis * Vector3::Dot(delta, xAxis) + yAxis * Vector3::Dot(delta, yAxis);
        break;
    case GizmoHandleId::YZ:
        move = yAxis * Vector3::Dot(delta, yAxis) + zAxis * Vector3::Dot(delta, zAxis);
        break;
    case GizmoHandleId::XZ:
        move = xAxis * Vector3::Dot(delta, xAxis) + zAxis * Vector3::Dot(delta, zAxis);
        break;
    case GizmoHandleId::XYZ:
        move = delta;
        break;
    default:
        return;
    }

    for (const TargetState& target : targets)
        target.transform->SetWorldPosition(target.initialWorldPosition + move);
}

void TranslationGizmoStrategy::EndDragging()
{
    selectedHandle = GizmoHandleId::None;
    targets.clear();
}

} // namespace URay
