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
    this->state = state;
    NotifyUpdated();
}

} // namespace URay::Render
