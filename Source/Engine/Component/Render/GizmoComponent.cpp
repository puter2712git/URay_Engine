#include "GizmoComponent.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Unit.h"

#include "Core/Math/Math.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"
#include "Engine/Material/MaterialManager.h"

#include <algorithm>

namespace URay
{

URAY_REGISTER_CLASS(GizmoComponent)

GizmoComponent::GizmoComponent()
{
    meshes[static_cast<size_t>(GizmoMode::Translation)] = gEngine->GetMeshManager()->GetMesh("arrow");
    meshes[static_cast<size_t>(GizmoMode::Rotation)] = gEngine->GetMeshManager()->GetMesh("rotation_gizmo");
    meshes[static_cast<size_t>(GizmoMode::Scale)] = gEngine->GetMeshManager()->GetMesh("scale_gizmo");

    material = gEngine->GetMaterialManager()->GetOrCreate("default");
}

void GizmoComponent::RegisterClass()
{
    Super::RegisterClass();
}

void GizmoComponent::Update(float deltaTime)
{
    if (!targetUnit || !targetUnit->GetTransform())
        return;

    UpdateGizmo();

    TransformComponent* transform = targetUnit->GetTransform();
    Vector3 targetPos = transform->GetPosition();

    CameraComponent* camera = gEngine->GetCamera();
    TransformComponent* camTransform = camera->GetOwner()->GetTransform();
    Vector3 camPos = camTransform->GetPosition();

    float toCameraLength = (targetPos - camPos).GetLength();
    constexpr float scaleFactor = 0.1f;
    constexpr float minScale = 0.25f;
    constexpr float maxScale = 10.0f;

    float gizmoScale = std::clamp(toCameraLength * scaleFactor, minScale, maxScale);

    Matrix targetT = Matrix::MakeTranslation(transform->GetPosition());
    Matrix targetR = Matrix::MakeRotation(transform->GetRotation());
    Matrix scaleMatrix = Matrix::MakeScale(Vector3(gizmoScale, gizmoScale, gizmoScale));

    Matrix targetWorld = scaleMatrix * targetR * targetT;

    matrices[0][0] = Matrix::MakeRotationZ(-90.0f) * targetWorld;
    matrices[0][1] = targetWorld;
    matrices[0][2] = Matrix::MakeRotationX(90.0f) * targetWorld;

    matrices[1][0] = Matrix::MakeRotationY(90.0f) * targetWorld;
    matrices[1][1] = Matrix::MakeRotationX(90.0f) * targetWorld;
    matrices[1][2] = targetWorld;

    matrices[2] = matrices[0];
}

void GizmoComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (IsEnabled())
        return;

    if (!targetUnit)
        return;

    Mesh* currMesh = GetCurrMesh();
    std::array<Matrix, static_cast<size_t>(Axis::Count)>& gizmoMatrices = matrices[GetCurrModeIndex()];

    GizmoCommandContext xCoord = {};
    xCoord.worldMatrix = gizmoMatrices[0];

    if (IsDragging())
    {
        xCoord.colorTint = selectedAxis == static_cast<int>(Axis::X) ? Color::Yellow : Color::Red;
    }
    else
    {
        xCoord.colorTint = hoveredAxis == static_cast<int>(Axis::X) ? Color::Yellow : Color::Red;
    }

    xCoord.vertexBuffer = currMesh->GetVertexBuffer();
    xCoord.vertexCount = static_cast<uint32_t>(currMesh->GetVertices().size());
    xCoord.indexBuffer = currMesh->GetIndexBuffer();
    xCoord.indexCount = static_cast<uint32_t>(currMesh->GetIndices().size());
    xCoord.material = material;

    GizmoCommandContext yCoord = {};
    yCoord.worldMatrix = gizmoMatrices[1];

    if (IsDragging())
    {
        yCoord.colorTint = selectedAxis == static_cast<int>(Axis::Y) ? Color::Yellow : Color::Green;
    }
    else
    {
        yCoord.colorTint = hoveredAxis == static_cast<int>(Axis::Y) ? Color::Yellow : Color::Green;
    }

    yCoord.vertexBuffer = currMesh->GetVertexBuffer();
    yCoord.vertexCount = static_cast<uint32_t>(currMesh->GetVertices().size());
    yCoord.indexBuffer = currMesh->GetIndexBuffer();
    yCoord.indexCount = static_cast<uint32_t>(currMesh->GetIndices().size());
    yCoord.material = material;

    GizmoCommandContext zCoord = {};
    zCoord.worldMatrix = gizmoMatrices[2];

    if (IsDragging())
    {
        zCoord.colorTint = selectedAxis == static_cast<int>(Axis::Z) ? Color::Yellow : Color::Blue;
    }
    else
    {
        zCoord.colorTint = hoveredAxis == static_cast<int>(Axis::Z) ? Color::Yellow : Color::Blue;
    }

    zCoord.vertexBuffer = currMesh->GetVertexBuffer();
    zCoord.vertexCount = static_cast<uint32_t>(currMesh->GetVertices().size());
    zCoord.indexBuffer = currMesh->GetIndexBuffer();
    zCoord.indexCount = static_cast<uint32_t>(currMesh->GetIndices().size());
    zCoord.material = material;

    builder.BuildFromGizmo(xCoord);
    builder.BuildFromGizmo(yCoord);
    builder.BuildFromGizmo(zCoord);
}

void GizmoComponent::StartDragging(const Vector2& clickPos, int selectedAxis)
{
    CameraComponent* camera = gEngine->GetCamera();
    TransformComponent* camTransform = camera->GetOwner()->GetTransform();

    SetDragging(true);
    SetSelectedAxis(selectedAxis);

    Vector3 cameraWorldFarPos = camera->ScreenToWorld(Vector3(clickPos.x, clickPos.y, 1.0f));
    dragStartWorldPos = camTransform->GetPosition();
    dragStartLineDir = cameraWorldFarPos - dragStartWorldPos;
    targetInitPos = targetUnit->GetTransform()->GetPosition();
    targetInitRotation = targetUnit->GetTransform()->GetRotation();
    targetInitScale = targetUnit->GetTransform()->GetScale();
    dragAxisDir = GetCurrAxisDir();
}

void GizmoComponent::EndDragging()
{
    SetDragging(false);
    SetSelectedAxis(-1);

    dragStartWorldPos = Vector3::Zero;
    dragStartLineDir = Vector3::Zero;
    targetInitPos = Vector3::Zero;
    targetInitRotation = Vector3::Zero;
    targetInitScale = Vector3::One;
    dragAxisDir = Vector3::Zero;
}

const TransformComponent* GizmoComponent::GetTargetTransform() const
{
    if (!targetUnit || !targetUnit->GetTransform())
        return nullptr;

    return targetUnit->GetTransform();
}

void GizmoComponent::UpdateGizmo()
{
    if (!IsDragging())
        return;

    switch (currMode)
    {
    case GizmoMode::Translation:
        UpdateGizmoTranslation();
        break;
    case GizmoMode::Rotation:
        UpdateGizmoRotation();
        break;
    case GizmoMode::Scale:
        UpdateGizmoScale();
        break;
    case GizmoMode::Count:
        break;
    }
}

void GizmoComponent::UpdateGizmoTranslation()
{
    InputManager inputManager = gEngine->GetInputManager();

    CameraComponent* camera = gEngine->GetCamera();
    TransformComponent* cameraTransform = camera->GetOwner()->GetTransform();
    Vector3 cameraForward = cameraTransform->GetForward();
    Vector3 axisDir = dragAxisDir;

    Vector3 currCameraFarPos = camera->ScreenToWorld(Vector3(inputManager.mouseX, inputManager.mouseY, 1.0f));
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

    float startAxisPos = Vector3::Dot(startHitPoint - targetInitPos, axisDir);
    float currAxisPos = Vector3::Dot(currHitPoint - targetInitPos, axisDir);

    float translationAmount = currAxisPos - startAxisPos;

    Vector3 movedPos = targetInitPos + axisDir * translationAmount;
    targetUnit->GetTransform()->SetPosition(movedPos);
}

void GizmoComponent::UpdateGizmoRotation()
{
    InputManager inputManager = gEngine->GetInputManager();
    CameraComponent* camera = gEngine->GetCamera();

    Vector3 startHitPoint;
    if (!GetDragPlaneHitPoint(dragStartLineDir, startHitPoint))
        return;

    Vector3 currCameraFarPos = camera->ScreenToWorld(Vector3(inputManager.mouseX, inputManager.mouseY, 1.0f));
    Vector3 currHitPoint;
    if (!GetDragPlaneHitPoint(currCameraFarPos - dragStartWorldPos, currHitPoint))
        return;

    Vector3 startVector = (startHitPoint - targetInitPos).GetNormalized();
    Vector3 currVector = (currHitPoint - targetInitPos).GetNormalized();
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

void GizmoComponent::UpdateGizmoScale()
{
    InputManager inputManager = gEngine->GetInputManager();
    CameraComponent* camera = gEngine->GetCamera();
    TransformComponent* cameraTransform = camera->GetOwner()->GetTransform();

    Vector3 planeNormal = Vector3::Cross(
                              dragAxisDir,
                              Vector3::Cross(cameraTransform->GetForward(), dragAxisDir).GetNormalized())
                              .GetNormalized();

    Vector3 startHitPoint;
    if (!Math::IntersectLinePlane(dragStartWorldPos, dragStartLineDir, targetInitPos, planeNormal, startHitPoint))
        return;

    Vector3 currCameraFarPos = camera->ScreenToWorld(Vector3(inputManager.mouseX, inputManager.mouseY, 1.0f));
    Vector3 currHitPoint;
    if (!Math::IntersectLinePlane(dragStartWorldPos, currCameraFarPos - dragStartWorldPos, targetInitPos, planeNormal, currHitPoint))
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

bool GizmoComponent::GetDragPlaneHitPoint(const Vector3& lineDir, Vector3& outHitPoint) const
{
    return Math::IntersectLinePlane(dragStartWorldPos, lineDir, targetInitPos, dragAxisDir, outHitPoint);
}

Vector3 GizmoComponent::GetCurrAxisDir()
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
