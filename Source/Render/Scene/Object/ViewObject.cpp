#include "ViewObject.h"

namespace URay::Render
{

ViewObject::ViewObject(const Matrix& worldMatrix, const Matrix& viewMatrix, const Matrix& projMatrix)
    : worldMatrix(worldMatrix), viewMatrix(viewMatrix), projMatrix(projMatrix)
{
}

ViewObject::~ViewObject() = default;

void ViewObject::Update(const ViewObjectState& state)
{
    worldMatrix = state.worldMatrix;
    viewMatrix = state.viewMatrix;
    projMatrix = state.projMatrix;
}

} // namespace URay::Render
