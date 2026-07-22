#include "GizmoComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Unit.h"

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

    Matrix targetWorld = targetUnit->GetTransform()->GetWorldMatrix();
    matrices[0] = Matrix::MakeRotationZ(-90.0f) * targetWorld;
    matrices[1] = targetWorld;
    matrices[2] = Matrix::MakeRotationX(90.0f) * targetWorld;
}

void GizmoComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (IsEnabled())
        return;

    if (!targetUnit)
        return;

    MeshCommandContext xCoord = {};
    xCoord.worldMatrix = matrices[0];
    xCoord.colorTint = hoveredAxis == static_cast<int>(Axis::X) ? Color::Yellow : Color::Red;
    xCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    xCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    xCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    xCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    xCoord.material = material;

    MeshCommandContext yCoord = {};
    yCoord.worldMatrix = matrices[1];
    yCoord.colorTint = hoveredAxis == static_cast<int>(Axis::Y) ? Color::Yellow : Color::Green;
    yCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    yCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    yCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    yCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    yCoord.material = material;

    MeshCommandContext zCoord = {};
    zCoord.worldMatrix = matrices[2];
    zCoord.colorTint = hoveredAxis == static_cast<int>(Axis::Z) ? Color::Yellow : Color::Blue;
    zCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    zCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    zCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    zCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    zCoord.material = material;

    builder.BuildFromMesh(xCoord);
    builder.BuildFromMesh(yCoord);
    builder.BuildFromMesh(zCoord);
}

const TransformComponent* GizmoComponent::GetTargetTransform() const
{
    if (!targetUnit || !targetUnit->GetTransform())
        return nullptr;

    return targetUnit->GetTransform();
}

} // namespace URay
