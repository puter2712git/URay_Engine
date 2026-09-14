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
    this->state = state;
}

} // namespace URay::Render
