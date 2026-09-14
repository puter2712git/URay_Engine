#include "FogObject.h"

namespace URay::Render
{

FogObject::FogObject(const FogObjectState& state)
    : state(state) {}

FogObject::~FogObject() = default;

void FogObject::Update(const FogObjectState& state)
{
    this->state = state;
}

} // namespace URay::Render
