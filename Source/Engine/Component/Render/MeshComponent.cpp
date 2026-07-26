#include "MeshComponent.h"

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

MeshComponent::MeshComponent()
{
    mesh = gEngine->GetMeshManager()->GetMesh("box");

    material = gEngine->GetMaterialManager()->GetOrCreate("default");
}

void MeshComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (!IsEnabled())
        return;

    TransformComponent* transform = GetOwner()->GetTransform();

    MeshCommandContext context = {};
    context.worldMatrix = transform->GetWorldMatrix();
    context.vertexBuffer = mesh->GetVertexBuffer();
    context.indexBuffer = mesh->GetIndexBuffer();
    context.indexCount = static_cast<uint32_t>(mesh->GetIndices().size());
    context.material = GetMaterial();

    builder.BuildFromMesh(context);
}

void MeshComponent::GetProperties(std::vector<Property>& properties)
{
    Component::GetProperties(properties);

    properties.emplace_back(PropertyType::Mesh, "Mesh", &mesh);
}

} // namespace URay
