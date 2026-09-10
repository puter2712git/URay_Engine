#include "PointLightObject.h"

namespace URay::Render
{

PointLightObject::PointLightObject(const PointLightObjectState& state)
{
    Update(state);
}

PointLightObject::~PointLightObject() = default;

void PointLightObject::Update(const PointLightObjectState& state)
{
    position = state.position;
    radius = state.radius;
    intensity = state.intensity;
    color = state.color;
}

} // namespace URay::Render
