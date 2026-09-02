#include "DecalObject.h"

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
    material = state.material;
}

} // namespace URay::Render
