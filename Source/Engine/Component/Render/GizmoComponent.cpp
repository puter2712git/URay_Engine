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

void GizmoComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (!IsEnabled())
        return;

    if (!targetUnit || !targetUnit->GetTransform())
        return;

    uint32_t objectId = gEngine->GetObjectId();

    Matrix targetWorld = targetUnit->GetTransform()->GetWorldMatrix();

    MeshCommandContext xCoord = {};
    xCoord.worldMatrix = Matrix::MakeRotationZ(-90.0f) * targetWorld;
    xCoord.colorTint = objectId == static_cast<uint32_t>(Axis::X) ? Color::Yellow : Color::Red;
    xCoord.objectId = static_cast<uint32_t>(Axis::X);
    xCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    xCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    xCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    xCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    xCoord.material = material;

    MeshCommandContext yCoord = {};
    yCoord.worldMatrix = targetWorld;
    yCoord.colorTint = objectId == static_cast<uint32_t>(Axis::Y) ? Color::Yellow : Color::Green;
    yCoord.objectId = static_cast<uint32_t>(Axis::Y);
    yCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    yCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    yCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    yCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    yCoord.material = material;

    MeshCommandContext zCoord = {};
    zCoord.worldMatrix = Matrix::MakeRotationX(90.0f) * targetWorld;
    zCoord.colorTint = objectId == static_cast<uint32_t>(Axis::Z) ? Color::Yellow : Color::Blue;
    zCoord.objectId = static_cast<uint32_t>(Axis::Z);
    zCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    zCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    zCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    zCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    zCoord.material = material;

    builder.BuildFromMesh(xCoord);
    builder.BuildFromMesh(yCoord);
    builder.BuildFromMesh(zCoord);
}

} // namespace URay
