#include "MeshComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Unit.h"

#include "Render/Material/MaterialManager.h"

namespace URay
{

MeshComponent::MeshComponent()
{
    mesh = gEngine->GetMeshManager()->GetMesh("box");

    material = gEngine->GetMaterialManager()->GetOrCreate("default");
}

DrawCommand MeshComponent::SubmitCommand()
{
    TransformComponent* transform = GetOwner()->GetTransform();
    DrawCommand cmd = {};
    cmd.worldMatrix = transform->GetWorldMatrix();
    cmd.vertexBuffer = mesh->GetVertexBuffer();
    cmd.indexBuffer = mesh->GetIndexBuffer();
    cmd.indexCount = static_cast<uint32_t>(mesh->GetIndices().size());
    cmd.material = GetMaterial();

    return cmd;
}

} // namespace URay
