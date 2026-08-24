#include "MeshObject.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay::RHI
{

MeshObject::MeshObject(const MeshObjectState& state)
{
    worldMatrix = state.worldMatrix;
    mesh = state.mesh;
    materials = state.materials;

    worldBounds = mesh->GetLocalBounds().Transform(worldMatrix);
}

MeshObject::~MeshObject() = default;

void MeshObject::Update(const MeshObjectState& state)
{
    worldMatrix = state.worldMatrix;
    mesh = state.mesh;
    materials = state.materials;

    worldBounds = state.mesh->GetLocalBounds().Transform(worldMatrix);
}

void MeshObject::Submit(DrawCommandBuilder& builder) const
{
    for (const auto& section : mesh->GetSections())
    {
        builder.BuildMesh({
            .worldMatrix = worldMatrix,
            .colorTint = Color::White,
            .mesh = mesh,
            .material = materials[section.materialIndex],
            .indexOffset = section.indexOffset,
            .indexCount = section.indexCount,
        });
    }
}

} // namespace URay::RHI
