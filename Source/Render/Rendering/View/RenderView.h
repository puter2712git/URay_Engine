#pragma once

#include "Render/Rendering/View/ViewMode.h"

#include "Core/Math/Matrix.h"
#include "Core/Math/Vector3.h"

namespace URay::Render
{

struct RenderView
{
    Vector3 cameraPosition = Vector3::Zero;
    Vector3 cameraDirection = Vector3::Zero;
    float nearPlane = 0.0f;
    float farPlane = 0.0f;

    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;

    ViewMode viewMode = ViewMode::Lit;
};

} // namespace URay::Render
