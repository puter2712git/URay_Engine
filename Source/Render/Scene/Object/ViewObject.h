#pragma once

#include "Render/Scene/Object/RenderObject.h"

#include "Core/Math/Matrix.h"

namespace URay::RHI
{

struct ViewObjectState
{
    Matrix worldMatrix = Matrix::Identity;
    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;
};

class ViewObject : public RenderObject
{
public:
    ViewObject(const Matrix& worldMatrix, const Matrix& viewMatrix, const Matrix& projMatrix);
    ~ViewObject() override;

public:
    void Update(const ViewObjectState& state);

private:
    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;
};

} // namespace URay::RHI
