#include "SpotLightObject.h"

namespace URay::Render
{

SpotLightObject::SpotLightObject(const SpotLightObjectState& state)
{
    Update(state);
}

SpotLightObject::~SpotLightObject() = default;

void SpotLightObject::Update(const SpotLightObjectState& state)
{
    position = state.position;
    direction = state.direction;
    range = state.range;
    innerConeAngle = state.innerConeAngle;
    outerConeAngle = state.outerConeAngle;
    intensity = state.intensity;
    color = state.color;
}

} // namespace URay::Render
