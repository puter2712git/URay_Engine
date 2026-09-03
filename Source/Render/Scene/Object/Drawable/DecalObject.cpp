#include "DecalObject.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay::Render
{

DecalObject::DecalObject(const DecalObjectState& state)
{
    Update(state);
}

DecalObject::~DecalObject() = default;

void DecalObject::Update(const DecalObjectState& state)
{
    worldMatrix = state.worldMatrix;
    worldBounds = state.localBounds.Transform(worldMatrix);
    localBounds = state.localBounds;
    boxMesh = state.boxMesh;
    material = state.material;
}

void DecalObject::Submit(DrawCommandBuilder& builder) const
{
    builder.BuildDecal({ .worldMatrix = worldMatrix,
                         .localBounds = localBounds,
                         .boxMesh = boxMesh,
                         .material = material });
}

} // namespace URay::Render
