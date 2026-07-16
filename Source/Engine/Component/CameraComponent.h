#pragma once

#include "Engine/Component/Component.h"

#include "Core/Math/Matrix.h"

namespace URay
{

class CameraComponent : public Component
{
public:
    void Update(float deltaTime) override;

    Matrix GetViewMatrix() const
    {
        return viewMatrix;
    }

    Matrix GetProjMatrix() const
    {
        return projMatrix;
    }

private:
    void UpdateViewMatrix();
    void UpdateProjMatrix();

private:
    float fov = 60.0f;

    float near = 0.1f;
    float far = 1000.0f;

    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;
};

} // namespace URay
