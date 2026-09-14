#include "ViewObject.h"

namespace URay::Render
{

ViewObject::ViewObject(const ViewObjectState& state)
    : state(state) {}

ViewObject::~ViewObject() = default;

void ViewObject::Update(const ViewObjectState& state)
{
    this->state = state;
}

} // namespace URay::Render
