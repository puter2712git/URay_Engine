#include "MeshComponent.h"

#include "Engine/Engine.h"
#include "Engine/Mesh/Mesh.h"

#include "Render/Material/MaterialManager.h"

namespace URay
{

MeshComponent::MeshComponent()
{
    mesh = new Mesh();

    material = gEngine->GetMaterialManager()->GetOrCreate("default");
}

DrawCommand MeshComponent::SubmitCommand()
{
    DrawCommand cmd = {};
    cmd.worldMatrix = GetWorldMatrix();
    cmd.vertexBuffer = mesh->GetVertexBuffer();
    cmd.indexBuffer = mesh->GetIndexBuffer();
    cmd.indexCount = static_cast<uint32_t>(mesh->GetIndices().size());
    cmd.material = GetMaterial();

    return cmd;
}

} // namespace URay
