#include "MeshObject.h"

#include "Render/Rendering/DrawCommand/DrawCommandBuilder.h"

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
    this->state = state;
    worldBounds = state.mesh ? state.mesh->GetLocalBounds().Transform(state.worldMatrix) : AABB{};
    NotifyUpdated();
}

void MeshObject::Submit(DrawCommandBuilder& builder) const
{
    if (!state.mesh)
        return;

    for (const auto& section : state.mesh->GetSections())
    {
        if (section.materialIndex >= state.materials.size() ||
            !state.materials[section.materialIndex])
            continue;

        builder.BuildMesh(
            { .worldMatrix = state.worldMatrix,
              .colorTint = state.colorTint,
              .mesh = state.mesh,
              .material = state.materials[section.materialIndex],
              .indexOffset = section.indexOffset,
              .indexCount = section.indexCount },
            RenderPassId::Opaque);

        if (state.castsShadow)
        {
            builder.BuildMesh(
                { .worldMatrix = state.worldMatrix,
                  .colorTint = state.colorTint,
                  .mesh = state.mesh,
                  .material = state.materials[section.materialIndex],
                  .indexOffset = section.indexOffset,
                  .indexCount = section.indexCount },
                RenderPassId::Shadow);
        }
    }
}

void MeshObject::SubmitSelectionMask(DrawCommandBuilder& builder) const
{
    if (!state.mesh)
        return;

    for (const auto& section : state.mesh->GetSections())
    {
        if (section.materialIndex >= state.materials.size() ||
            !state.materials[section.materialIndex])
            continue;

        builder.BuildMesh(
            { .worldMatrix = state.worldMatrix,
              .colorTint = state.colorTint,
              .mesh = state.mesh,
              .material = state.materials[section.materialIndex],
              .indexOffset = section.indexOffset,
              .indexCount = section.indexCount },
            RenderPassId::SelectionMask);
    }
}

} // namespace URay::Render
