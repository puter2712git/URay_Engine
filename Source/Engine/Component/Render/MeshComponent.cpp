#include "MeshComponent.h"

#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Object/Class/Class.h"
#include "Engine/Unit.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

using namespace RHI;

URAY_REGISTER_CLASS(MeshComponent)
URAY_REGISTER_COMPONENT(MeshComponent)

MeshComponent::MeshComponent()
{
    mesh = gEngine->GetMeshManager()->GetMesh("Box");
    materials.push_back(gEngine->GetMaterialManager()->GetOrCreate("Mesh"));
}

void MeshComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ PropertyType::Mesh, "Mesh", offsetof(MeshComponent, mesh), sizeof(Mesh*) });
}

void MeshComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (!IsEnabled())
        return;

    TransformComponent* transform = GetOwner()->GetTransform();

    for (size_t i = 0; i < materials.size(); ++i)
    {
        MeshCommandContext context = {};
        context.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
        context.meshAsset = GetMesh();
        context.material = GetMaterial(i);

        builder.BuildFromMesh(context);
    }
}

} // namespace URay
