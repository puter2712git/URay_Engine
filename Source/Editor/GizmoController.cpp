#include "GizmoController.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Unit.h"

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

void GizmoController::StartDragging(const Vector2& clickPos, int selectedAxis)
{
}

} // namespace URay
