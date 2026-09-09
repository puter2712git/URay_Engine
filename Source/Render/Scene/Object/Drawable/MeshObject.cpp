#include "MeshObject.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"

#include "Engine/Asset/Material/Material.h"

namespace URay::Render
{

MeshObject::MeshObject(const MeshObjectState& state)
{
    Update(state);
}

MeshObject::~MeshObject() = default;

void MeshObject::Update(const MeshObjectState& state)
{
    worldMatrix = state.worldMatrix;
    colorTint = state.colorTint;
    mesh = state.mesh;
    materials = state.materials;
    worldBounds = mesh ? mesh->GetLocalBounds().Transform(worldMatrix) : AABB{};
}

void MeshObject::Submit(DrawCommandBuilder& builder) const
{
    if (!mesh)
        return;

    for (const auto& section : mesh->GetSections())
    {
        if (section.materialIndex >= materials.size() ||
            !materials[section.materialIndex])
            continue;

        builder.BuildMesh({
            .worldMatrix = worldMatrix,
            .colorTint = colorTint,
            .mesh = mesh,
            .material = materials[section.materialIndex],
            .indexOffset = section.indexOffset,
            .indexCount = section.indexCount,
        });
    }
}

} // namespace URay::Render
