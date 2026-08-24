#include "ViewObject.h"

namespace URay::Render
{

ViewObject::ViewObject(const Matrix& worldMatrix, const Matrix& viewMatrix, const Matrix& projMatrix)
    : viewMatrix(viewMatrix), projMatrix(projMatrix)
{
    this->worldMatrix = worldMatrix;
}

ViewObject::~ViewObject() = default;

void ViewObject::Update(const ViewObjectState& state)
{
    worldMatrix = state.worldMatrix;
    viewMatrix = state.viewMatrix;
    projMatrix = state.projMatrix;
}

} // namespace URay::Render
