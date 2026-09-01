#include "FogObject.h"

namespace URay::Render
{

FogObject::FogObject(const FogObjectState& state)
{
    fogColor = state.fogColor;
    fogStart = state.fogStart;
    fogEnd = state.fogEnd;
    density = state.density;
}

FogObject::~FogObject() = default;

void FogObject::Update(const FogObjectState& state)
{
    fogColor = state.fogColor;
    fogStart = state.fogStart;
    fogEnd = state.fogEnd;
    density = state.density;
}

} // namespace URay::Render
