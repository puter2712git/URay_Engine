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
    this->state = state;
    NotifyUpdated();
}

} // namespace URay::Render
