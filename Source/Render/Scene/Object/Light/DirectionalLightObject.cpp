#include "DirectionalLightObject.h"

namespace URay::Render
{

DirectionalLightObject::DirectionalLightObject(const DirectionalLightObjectState& state)
{
    Update(state);
}

DirectionalLightObject::~DirectionalLightObject() = default;

void DirectionalLightObject::Update(const DirectionalLightObjectState& state)
{
    direction = state.direction;
    intensity = state.intensity;
    color = state.color;
}

} // namespace URay::Render
