#include "MeshComponent.h"

#include "Engine/Mesh/Mesh.h"

namespace URay
{

MeshComponent::MeshComponent()
{
    mesh = new Mesh();
}

DrawCommand MeshComponent::SubmitCommand()
{
    DrawCommand cmd = {};
    cmd.worldMatrix = GetWorldMatrix();
    cmd.vertexBuffer = mesh->GetVertexBuffer();
    cmd.indexBuffer = mesh->GetIndexBuffer();
    cmd.indexCount = static_cast<uint32_t>(mesh->GetIndices().size());

    return cmd;
}

} // namespace URay
