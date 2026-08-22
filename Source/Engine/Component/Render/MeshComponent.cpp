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

#include <algorithm>

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

void MeshComponent::OnAttached()
{
    UpdateWorldBounds();

    Unit* owner = GetOwner();
    owner->RegisterTransformUpdateCallback([this]()
                                           { UpdateWorldBounds(); });
}

void MeshComponent::OnDetached()
{
}

void MeshComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (!IsEnabled())
        return;

    TransformComponent* transform = GetOwner()->GetTransform();

    const std::vector<MeshSection>& sections = mesh->GetSections();
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

    UpdateWorldBounds();
}

void MeshComponent::SetMaterial(Material* newMaterial, size_t index)
{
    if (materials.size() <= index)
    {
        materials.resize(index + 1);
    }

    materials[index] = newMaterial;
}

void MeshComponent::UpdateWorldBounds()
{
    if (!mesh)
    {
        worldBounds = {};
        return;
    }

    Unit* owner = GetOwner();
    if (!owner)
    {
        worldBounds = {};
        return;
    }

    TransformComponent* transform = owner->GetTransform();
    const AABB& localBounds = mesh->GetLocalBounds();

    const Vector3 corners[8] = {
        { localBounds.min.x, localBounds.min.y, localBounds.min.z },
        { localBounds.min.x, localBounds.min.y, localBounds.max.z },
        { localBounds.min.x, localBounds.max.y, localBounds.min.z },
        { localBounds.min.x, localBounds.max.y, localBounds.max.z },
        { localBounds.max.x, localBounds.min.y, localBounds.min.z },
        { localBounds.max.x, localBounds.min.y, localBounds.max.z },
        { localBounds.max.x, localBounds.max.y, localBounds.min.z },
        { localBounds.max.x, localBounds.max.y, localBounds.max.z },
    };

    const Vector3 first = transform
                              ? transform->TransformPoint(corners[0])
                              : corners[0];

    worldBounds.min = first;
    worldBounds.max = first;

    for (size_t i = 1; i < std::size(corners); ++i)
    {
        const Vector3 p = transform
                              ? transform->TransformPoint(corners[i])
                              : corners[i];

        worldBounds.min.x = std::min(worldBounds.min.x, p.x);
        worldBounds.min.y = std::min(worldBounds.min.y, p.y);
        worldBounds.min.z = std::min(worldBounds.min.z, p.z);
        worldBounds.max.x = std::max(worldBounds.max.x, p.x);
        worldBounds.max.y = std::max(worldBounds.max.y, p.y);
        worldBounds.max.z = std::max(worldBounds.max.z, p.z);
    }
}

} // namespace URay
