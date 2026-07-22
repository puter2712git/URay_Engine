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
    MeshCommandContext context = {};
    context.worldMatrix = Matrix::Identity;
    context.colorTint = Color::Red;
    context.vertexBuffer = meshes[0]->GetVertexBuffer();
    context.vertexCount = static_cast<uint32_t>(meshes[0]->GetVertices().size());
    context.indexBuffer = meshes[0]->GetIndexBuffer();
    context.indexCount = static_cast<uint32_t>(meshes[0]->GetIndices().size());
    context.material = material;

    builder.BuildFromMesh(context);
}

} // namespace URay
