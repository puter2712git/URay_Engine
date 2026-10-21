#include "SkinnedMeshObject.h"

#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Mesh/SkinnedMesh.h>
#include <Render/Rendering/DrawCommand/DrawCommandBuilder.h>

namespace URay::Render
{

SkinnedMeshObject::SkinnedMeshObject(const SkinnedMeshObjectState& state)
{
    Update(state);
}

SkinnedMeshObject::~SkinnedMeshObject() = default;

void SkinnedMeshObject::Update(const SkinnedMeshObjectState& state)
{
    this->state = state;
    worldBounds = state.skinnedMesh ? state.skinnedMesh->GetLocalBounds().Transform(state.worldMatrix) : AABB{};
    NotifyUpdated();
}

void SkinnedMeshObject::Submit(DrawCommandBuilder& builder, const DrawCommandParams& params) const
{
    if (!state.skinnedMesh)
        return;

    for (const auto& section : state.skinnedMesh->GetSections())
    {
        if (section.materialIndex >= state.materials.size() ||
            !state.materials[section.materialIndex])
            continue;

        builder.BuildSkinnedMesh(
            { .worldMatrix = state.worldMatrix,
              .colorTint = state.colorTint,
              .objectId = GetId(),
              .skinnedMesh = state.skinnedMesh,
              .material = state.materials[section.materialIndex],
              .indexOffset = section.indexOffset,
              .indexCount = section.indexCount },
            RenderPassId::Opaque,
            params);

        if (state.castsShadow)
        {
            builder.BuildSkinnedMesh(
                { .worldMatrix = state.worldMatrix,
                  .colorTint = state.colorTint,
                  .objectId = GetId(),
                  .skinnedMesh = state.skinnedMesh,
                  .material = state.materials[section.materialIndex],
                  .indexOffset = section.indexOffset,
                  .indexCount = section.indexCount },
                RenderPassId::Shadow,
                params);
        }
    }
}

void SkinnedMeshObject::SubmitSelectionMask(DrawCommandBuilder& builder, const DrawCommandParams& params) const
{
    if (!state.skinnedMesh)
        return;

    for (const auto& section : state.skinnedMesh->GetSections())
    {
        if (section.materialIndex >= state.materials.size() ||
            !state.materials[section.materialIndex])
        {
            continue;
        }

        builder.BuildSkinnedMesh(
            { .worldMatrix = state.worldMatrix,
              .colorTint = state.colorTint,
              .objectId = GetId(),
              .skinnedMesh = state.skinnedMesh,
              .material = state.materials[section.materialIndex],
              .indexOffset = section.indexOffset,
              .indexCount = section.indexCount },
            RenderPassId::SelectionMask,
            params);
    }
}

} // namespace URay::Render
