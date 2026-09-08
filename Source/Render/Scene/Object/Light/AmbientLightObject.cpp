#include "AmbientLightObject.h"

namespace URay::Render
{

AmbientLightObject::AmbientLightObject(const AmbientLightObjectState& state)
{
    Update(state);
}

AmbientLightObject::~AmbientLightObject() = default;

void AmbientLightObject::Update(const AmbientLightObjectState& state)
{
    intensity = state.intensity;
    color = state.color;
}

} // namespace URay::Render
