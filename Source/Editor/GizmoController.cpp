#include "GizmoController.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Unit.h"

#include "Core/Input/InputManager.h"
#include "Core/Math/Math.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

#include <algorithm>

namespace URay
{

using namespace RHI;

GizmoController::GizmoController(MeshManager& meshManager, MaterialManager& materialManager)
{
    meshes[static_cast<size_t>(GizmoMode::Translation)] = meshManager.GetMesh("Arrow");
    meshes[static_cast<size_t>(GizmoMode::Rotation)] = meshManager.GetMesh("RotationGizmo");
    meshes[static_cast<size_t>(GizmoMode::Scale)] = meshManager.GetMesh("ScaleGizmo");

    material = materialManager.GetOrCreate("Mesh");
}

void GizmoController::Update(CameraComponent* camera)
{
    if (!targetUnit || !targetUnit->GetTransform())
        return;

    UpdateGizmo(camera);

    const TransformComponent* transform = targetUnit->GetTransform();
    const Vector3 targetPosition = transform->GetPosition();

    const TransformComponent* cameraTransform = camera->GetOwner()->GetTransform();
    const Vector3 cameraPosition = cameraTransform->GetPosition();

    const float toCameraLength = (targetPosition - cameraPosition).GetLength();

    float gizmoScale = std::clamp(toCameraLength * scaleFactor, minScale, maxScale);

    const Matrix T = Matrix::MakeTranslation(transform->GetPosition());
    const Matrix R = Matrix::MakeRotation(transform->GetRotation());
    const Matrix S = Matrix::MakeScale(Vector3(gizmoScale, gizmoScale, gizmoScale));

    const Matrix targetWorldMatrix = S * R * T;

    matrices[0][0] = Matrix::MakeRotation(-90.0f) * targetWorldMatrix;
    matrices[0][1] = targetWorldMatrix;
    matrices[0][2] = Matrix::MakeRotationX(90.0f) * targetWorldMatrix;

    matrices[1][0] = Matrix::MakeRotationY(90.0f) * targetWorldMatrix;
    matrices[1][1] = Matrix::MakeRotationX(90.0f) * targetWorldMatrix;
    matrices[1][2] = targetWorldMatrix;

    matrices[2] = matrices[0];
}

void GizmoController::Draw(DrawCommandBuilder& builder)
{
    if (!targetUnit || !targetUnit->GetTransform())
        return;

    Mesh* mesh = GetMesh();
    std::array<Matrix, static_cast<size_t>(Axis::Count)>& gizmoMatrices = matrices[GetModeIndex()];

    GizmoCommandContext xCoordContext = {};
    xCoordContext.worldMatrix = gizmoMatrices[0];

    if (IsDragging())
    {
        xCoordContext.colorTint = selectedAxis == static_cast<int>(Axis::X)
                                      ? Color::Yellow
                                      : Color::Red;
    }
    else
    {
        xCoordContext.colorTint = hoveredAxis == static_cast<int>(Axis::X)
                                      ? Color::Yellow
                                      : Color::Red;
    }

    xCoordContext.meshAsset = mesh;
    xCoordContext.material = material;

    GizmoCommandContext yCoordContext = {};
    yCoordContext.worldMatrix = gizmoMatrices[1];

    if (IsDragging())
    {
        yCoordContext.colorTint = selectedAxis == static_cast<int>(Axis::Y)
                                      ? Color::Yellow
                                      : Color::Green;
    }
    else
    {
        yCoordContext.colorTint = hoveredAxis == static_cast<int>(Axis::Y)
                                      ? Color::Yellow
                                      : Color::Green;
    }

    yCoordContext.meshAsset = mesh;
    yCoordContext.material = material;

    GizmoCommandContext zCoordContext = {};
    zCoordContext.worldMatrix = gizmoMatrices[2];

    if (IsDragging())
    {
        zCoordContext.colorTint = selectedAxis == static_cast<int>(Axis::Z)
                                      ? Color::Yellow
                                      : Color::Blue;
    }
    else
    {
        zCoordContext.colorTint = hoveredAxis == static_cast<int>(Axis::Z)
                                      ? Color::Yellow
                                      : Color::Blue;
    }

    zCoordContext.meshAsset = mesh;
    zCoordContext.material = material;

    builder.BuildFromGizmo(xCoordContext);
    builder.BuildFromGizmo(yCoordContext);
    builder.BuildFromGizmo(zCoordContext);
}

void GizmoController::StartDragging(const Vector2& clickPos, int selectedAxis, CameraComponent* camera)
{
    TransformComponent* cameraTransform = camera->GetOwner()->GetTransform();

    isDragging = true;
    SetSelectedAxis(selectedAxis);

    const Vector3 cameraWorldFarPos = camera->ScreenToWorld(Vector3(clickPos.x, clickPos.y, 1.0f));
    dragStartWorldPos = cameraTransform->GetPosition();
    dragStartLineDir = cameraWorldFarPos - dragStartWorldPos;
    targetInitPosition = targetUnit->GetTransform()->GetPosition();
    targetInitRotation = targetUnit->GetTransform()->GetRotation();
    targetInitScale = targetUnit->GetTransform()->GetScale();
    dragAxisDir = GetCurrAxisDir();
}

void GizmoController::EndDragging()
{
    isDragging = false;
    SetSelectedAxis(-1);

    dragStartWorldPos = Vector3::Zero;
    dragStartLineDir = Vector3::Zero;
    targetInitPosition = Vector3::Zero;
    targetInitRotation = Vector3::Zero;
    targetInitScale = Vector3::One;
    dragAxisDir = Vector3::Zero;
}

const TransformComponent* GizmoController::GetTargetTransform() const
{
    if (!targetUnit || !targetUnit->GetTransform())
        return nullptr;

    return targetUnit->GetTransform();
}

void GizmoController::UpdateGizmo(CameraComponent* camera)
{
    if (!IsDragging())
        return;

    switch (mode)
    {
    case GizmoMode::Translation:
        UpdateTranslation(camera);
        break;
    case GizmoMode::Rotation:
        UpdateRotation(camera);
        break;
    case GizmoMode::Scale:
        UpdateScale(camera);
        break;
    default:
        break;
    }
}

void GizmoController::UpdateTranslation(CameraComponent* camera)
{
    InputManager& input = gEngine->GetInputManager();

    TransformComponent* cameraTransform = camera->GetOwner()->GetTransform();
    Vector3 cameraForward = cameraTransform->GetForward();
    Vector3 axisDir = dragAxisDir;

    Vector3 currCameraFarPos = camera->ScreenToWorld(Vector3(input.mouseX, input.mouseY, 1.0f));
    Vector3 currLineDir = currCameraFarPos - dragStartWorldPos;

    Vector3 planeNormal = Vector3::Cross(axisDir, Vector3::Cross(cameraForward, axisDir).GetNormalized()).GetNormalized();

    Vector3 startHitPoint;
    Vector3 currHitPoint;

    Math::IntersectLinePlane(
        dragStartWorldPos, dragStartLineDir,
        targetUnit->GetTransform()->GetPosition(), planeNormal,
        startHitPoint);
    Math::IntersectLinePlane(
        dragStartWorldPos, currLineDir,
        targetUnit->GetTransform()->GetPosition(), planeNormal,
        currHitPoint);

    float startAxisPos = Vector3::Dot(startHitPoint - targetInitPosition, axisDir);
    float currAxisPos = Vector3::Dot(currHitPoint - targetInitPosition, axisDir);

    float translationAmount = currAxisPos - startAxisPos;

    Vector3 movedPos = targetInitPosition + axisDir * translationAmount;
    targetUnit->GetTransform()->SetPosition(movedPos);
}

void GizmoController::UpdateRotation(CameraComponent* camera)
{
    InputManager& input = gEngine->GetInputManager();

    Vector3 startHitPoint;
    if (!GetDragPlaneHitPoint(dragStartLineDir, startHitPoint))
        return;

    Vector3 currCameraFarPos = camera->ScreenToWorld(Vector3(input.mouseX, input.mouseY, 1.0f));
    Vector3 currHitPoint;
    if (!GetDragPlaneHitPoint(currCameraFarPos - dragStartWorldPos, currHitPoint))
        return;

    Vector3 startVector = (startHitPoint - targetInitPosition).GetNormalized();
    Vector3 currVector = (currHitPoint - targetInitPosition).GetNormalized();
    if (startVector.GetSquaredLength() < Math::EPSILON || currVector.GetSquaredLength() < Math::EPSILON)
        return;

    float sinAngle = Vector3::Dot(dragAxisDir, Vector3::Cross(startVector, currVector));
    float cosAngle = Vector3::Dot(startVector, currVector);
    float angleDegrees = Math::RadToDeg(std::atan2(sinAngle, cosAngle));

    Vector3 rotation = targetInitRotation;
    switch (selectedAxis)
    {
    case static_cast<int>(Axis::X):
        rotation.x += angleDegrees;
        break;
    case static_cast<int>(Axis::Y):
        rotation.y += angleDegrees;
        break;
    case static_cast<int>(Axis::Z):
        rotation.z += angleDegrees;
        break;
    default:
        return;
    }

    targetUnit->GetTransform()->SetRotation(rotation);
}

void GizmoController::UpdateScale(CameraComponent* camera)
{
    InputManager& input = gEngine->GetInputManager();

    TransformComponent* cameraTransform = camera->GetOwner()->GetTransform();

    Vector3 planeNormal = Vector3::Cross(
                              dragAxisDir,
                              Vector3::Cross(cameraTransform->GetForward(), dragAxisDir).GetNormalized())
                              .GetNormalized();

    Vector3 startHitPoint;
    if (!Math::IntersectLinePlane(dragStartWorldPos, dragStartLineDir, targetInitPosition, planeNormal, startHitPoint))
        return;

    Vector3 currCameraFarPos = camera->ScreenToWorld(Vector3(input.mouseX, input.mouseY, 1.0f));
    Vector3 currHitPoint;
    if (!Math::IntersectLinePlane(dragStartWorldPos, currCameraFarPos - dragStartWorldPos, targetInitPosition, planeNormal, currHitPoint))
        return;

    float amount = Vector3::Dot(currHitPoint - startHitPoint, dragAxisDir);
    constexpr float minimumScale = 0.01f;

    Vector3 scale = targetInitScale;
    switch (selectedAxis)
    {
    case static_cast<int>(Axis::X):
        scale.x = std::max(minimumScale, targetInitScale.x + amount);
        break;
    case static_cast<int>(Axis::Y):
        scale.y = std::max(minimumScale, targetInitScale.y + amount);
        break;
    case static_cast<int>(Axis::Z):
        scale.z = std::max(minimumScale, targetInitScale.z + amount);
        break;
    default:
        return;
    }

    targetUnit->GetTransform()->SetScale(scale);
}

bool GizmoController::GetDragPlaneHitPoint(const Vector3& lineDir, Vector3& outHitPoint) const
{
    return Math::IntersectLinePlane(dragStartWorldPos, lineDir, targetInitPosition, dragAxisDir, outHitPoint);
}

Vector3 GizmoController::GetCurrAxisDir()
{
    if (selectedAxis == -1)
        return Vector3::Zero;

    Vector3 axisDir = Vector3::Zero;
    switch (selectedAxis)
    {
    case 0:
        axisDir = Vector3::Right;
        break;
    case 1:
        axisDir = Vector3::Forward;
        break;
    case 2:
        axisDir = Vector3::Up;
        break;
    }

    axisDir = GetTargetTransform()->TransformVectorNoScale(axisDir);
    return axisDir;
}

} // namespace URay
