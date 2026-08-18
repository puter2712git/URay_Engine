#include "MeshComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
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
    Mesh* defaultMesh = gEngine->GetMeshManager()->GetMesh("Box");
    SetMesh(defaultMesh);
}

void MeshComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::Mesh,
                                 .name = "Mesh",
                                 .offset = offsetof(MeshComponent, mesh),
                                 .size = sizeof(Mesh*),
                                 .OnChangedCallback = [](Object* owner, const Property&)
                                 {
                                     MeshComponent* meshComp = static_cast<MeshComponent*>(owner);
                                     meshComp->SetMesh(meshComp->GetMesh());
                                 } });
}

void MeshComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (!IsEnabled())
        return;

    TransformComponent* transform = GetOwner()->GetTransform();

    std::vector<MeshSection> sections = mesh->GetSections();
    for (size_t sectionIndex = 0; sectionIndex < sections.size(); ++sectionIndex)
    {
        const MeshSection& section = sections[sectionIndex];

        Material* material = GetMaterial(section.materialIndex);
        if (!material)
            continue;

        builder.BuildFromMesh({
            .worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity,
            .mesh = mesh,
            .material = material,
            .indexOffset = section.indexOffset,
            .indexCount = section.indexCount,
        });
    }
}

void MeshComponent::SetMesh(Mesh* newMesh)
{
    mesh = newMesh;
    materials = mesh ? mesh->GetDefaultMaterials()
                     : std::vector<Material*>();
}

void MeshComponent::SetMaterial(Material* newMaterial, size_t index)
{
    if (materials.size() <= index)
    {
        materials.resize(index + 1);
    }

    materials[index] = newMaterial;
}

} // namespace URay
