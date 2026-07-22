#include "GizmoComponent.h"

#include "Engine/Engine.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"

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
    MeshCommandContext xCoord = {};
    xCoord.worldMatrix = Matrix::MakeRotationZ(-90.0f);
    xCoord.colorTint = Color::Red;
    xCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    xCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    xCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    xCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    xCoord.material = material;

    MeshCommandContext yCoord = {};
    yCoord.worldMatrix = Matrix::Identity;
    yCoord.colorTint = Color::Green;
    yCoord.vertexBuffer = meshes[0]->GetVertexBuffer();
    yCoord.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    yCoord.indexBuffer = meshes[0]->GetIndexBuffer();
    yCoord.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    yCoord.material = material;

    MeshCommandContext zCoord = {};
    zCoord.worldMatrix = Matrix::MakeRotationX(90.0f);
    zCoord.colorTint = Color::Blue;
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
