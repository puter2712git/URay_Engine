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
#include "Render/Material/MaterialManager.h"

namespace URay
{

GizmoComponent::GizmoComponent()
{
    meshes[static_cast<size_t>(GizmoMode::Translation)] = gEngine->GetMeshManager()->GetMesh("arrow");

    material = gEngine->GetMaterialManager()->GetOrCreate("default");
}

void GizmoComponent::Update(float deltaTime)
{
    if (!targetUnit || !targetUnit->GetTransform())
        return;

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

    Matrix targetWorldNoScale = scaleMatrix * targetR * targetT;

    matrices[0] = Matrix::MakeRotationZ(-90.0f) * targetWorldNoScale;
    matrices[1] = targetWorldNoScale;
    matrices[2] = Matrix::MakeRotationX(90.0f) * targetWorldNoScale;

    UpdateGizmo();
}

void GizmoComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (IsEnabled())
        return;

    if (!targetUnit)
        return;

    GizmoCommandContext xCoord = {};
    xCoord.worldMatrix = matrices[0];

    if (IsDragging())
    {
        xCoord.colorTint = selectedAxis == static_cast<int>(Axis::X) ? Color::Yellow : Color::Red;
    }
    else
    {
        xCoord.colorTint = hoveredAxis == static_cast<int>(Axis::X) ? Color::Yellow : Color::Red;
    }

    xCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    xCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    xCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    xCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    xCoord.material = material;

    GizmoCommandContext yCoord = {};
    yCoord.worldMatrix = matrices[1];

    if (IsDragging())
    {
        yCoord.colorTint = selectedAxis == static_cast<int>(Axis::Y) ? Color::Yellow : Color::Green;
    }
    else
    {
        yCoord.colorTint = hoveredAxis == static_cast<int>(Axis::Y) ? Color::Yellow : Color::Green;
    }

    yCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    yCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    yCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    yCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    yCoord.material = material;

    GizmoCommandContext zCoord = {};
    zCoord.worldMatrix = matrices[2];

    if (IsDragging())
    {
        zCoord.colorTint = selectedAxis == static_cast<int>(Axis::Z) ? Color::Yellow : Color::Blue;
    }
    else
    {
        zCoord.colorTint = hoveredAxis == static_cast<int>(Axis::Z) ? Color::Yellow : Color::Blue;
    }

    zCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    zCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    zCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    zCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
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
}

void GizmoComponent::EndDragging()
{
    SetDragging(false);
    SetSelectedAxis(-1);

    dragStartWorldPos = Vector3::Zero;
    dragStartLineDir = Vector3::Zero;
    targetInitPos = Vector3::Zero;
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

    InputManager inputManager = gEngine->GetInputManager();

    CameraComponent* camera = gEngine->GetCamera();
    TransformComponent* cameraTransform = camera->GetOwner()->GetTransform();
    Vector3 cameraForward = cameraTransform->GetForward();
    Vector3 axisDir = GetCurrAxisDir();

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
