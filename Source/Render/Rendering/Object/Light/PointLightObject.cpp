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
    this->state = state;
    NotifyUpdated();
}

} // namespace URay::Render
