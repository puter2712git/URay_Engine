#include "RotationGizmoStrategy.h"

#include "Editor/Gizmo/TransformGizmo.h"
#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"
#include "Core/Math/Math.h"

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
    if (!transform || !cameraTransform) return;
    selectedHandle = id; startWorldPosition = cameraTransform->GetPosition();
    startLineDirection = camera.ScreenToWorld(Vector3(position.x, position.y, 1.0f)) - startWorldPosition;
    targetInitPosition = transform->GetPosition(); targetInitRotation = transform->GetRotation();
    axisDirection = transform->TransformVectorNoScale(id == GizmoHandleId::X ? Vector3::Right : id == GizmoHandleId::Y ? Vector3::Forward : Vector3::Up).GetNormalized();
}

void RotationGizmoStrategy::UpdateDragging(const Vector2& position, const CameraComponent& camera)
{
    TransformComponent* transform = gizmo.GetTargetTransform();
    if (!transform) return;
    Vector3 startHit, currentHit;
    if (!Math::IntersectLinePlane(startWorldPosition, startLineDirection, targetInitPosition, axisDirection, startHit) ||
        !Math::IntersectLinePlane(startWorldPosition, camera.ScreenToWorld(Vector3(position.x, position.y, 1.0f)) - startWorldPosition, targetInitPosition, axisDirection, currentHit)) return;
    const Vector3 a = (startHit - targetInitPosition).GetNormalized();
    const Vector3 b = (currentHit - targetInitPosition).GetNormalized();
    if (a.GetSquaredLength() < Math::EPSILON || b.GetSquaredLength() < Math::EPSILON) return;
    const float angle = Math::RadToDeg(std::atan2(Vector3::Dot(axisDirection, Vector3::Cross(a, b)), Vector3::Dot(a, b)));
    Vector3 rotation = targetInitRotation;
    if (selectedHandle == GizmoHandleId::X) rotation.x += angle;
    else if (selectedHandle == GizmoHandleId::Y) rotation.y += angle;
    else if (selectedHandle == GizmoHandleId::Z) rotation.z += angle;
    transform->SetRotation(rotation);
}
} // namespace URay
