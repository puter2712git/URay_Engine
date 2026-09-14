#include "BillboardObject.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"

#include "Engine/Asset/Material/Material.h"

namespace URay::Render
{

BillboardObject::BillboardObject(const BillboardObjectState& state)
{
    Update(state);
}

BillboardObject::~BillboardObject() = default;

void BillboardObject::Update(const BillboardObjectState& state)
{
    this->state = state;
    worldBounds = state.mesh ? state.mesh->GetLocalBounds().Transform(state.worldMatrix) : AABB{};
}

void BillboardObject::Submit(DrawCommandBuilder& builder) const
{
    if (!state.mesh)
        return;

    for (const auto& section : state.mesh->GetSections())
    {
        if (section.materialIndex >= state.materials.size() ||
            !state.materials[section.materialIndex])
            continue;

        builder.BuildBillboard({
            .worldMatrix = state.worldMatrix,
            .colorTint = state.colorTint,
            .mesh = state.mesh,
            .material = state.materials[section.materialIndex],
            .indexOffset = section.indexOffset,
            .indexCount = section.indexCount,
        });
    }
}

} // namespace URay::Render
