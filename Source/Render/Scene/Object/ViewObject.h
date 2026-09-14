#pragma once

#include "Render/Scene/Object/RenderObject.h"

#include "Core/Math/Matrix.h"

namespace URay::Render
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
    ViewObject(const ViewObjectState& state);
    ~ViewObject() override;

public:
    void Update(const ViewObjectState& state);

    const Matrix& GetViewMatrix() const { return state.viewMatrix; }
    const Matrix& GetProjMatrix() const { return state.projMatrix; }

private:
    ViewObjectState state;
};

} // namespace URay::Render
