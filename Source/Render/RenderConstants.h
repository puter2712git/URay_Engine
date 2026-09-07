#pragma once

#include "Core/Math/Color.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Type/Types.h"

namespace URay::Render
{

struct ObjectConstants
{
    Matrix world = Matrix::Identity;
    Color colorTint = Color::White;
    uint32 objectId = 0;
};

struct FrameConstants
{
    Matrix view = Matrix::Identity;
    Matrix invView = Matrix::Identity;
    Matrix proj = Matrix::Identity;
    Matrix invProj = Matrix::Identity;
    Matrix viewProj = Matrix::Identity;
    Matrix invViewProj = Matrix::Identity;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    Vector2 renderTargetSize = Vector2::Zero;
};

} // namespace URay::Render
