#pragma once

#include "Core/Math/Color.h"
#include "Core/Math/Color3.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Math/Vector4.h"
#include "Core/Type/Types.h"

namespace URay::Render
{

struct ObjectConstants
{
    Matrix world = Matrix::Identity;
    Color colorTint = Color::White;
    uint32 objectId = 0;
};

struct AmbientLightConstants
{
    float intensity = 0.0f;
    Color3 color = Color3::White;
};

struct DirectionalLightConstants
{
    Vector3 direction = Vector3::Forward;
    float intensity = 0.0f;
    Color color = Color::White;
    uint32 castShadow = 0;
    float padding[3];
};

struct PointLightConstants
{
    Vector3 position = Vector3::Zero;
    float radius = 0.0f;
    float intensity = 0.0f;
    Color3 color = Color3::White;
    uint32 shadowIndex = UINT32_MAX;
    float padding[3];
};

struct SpotLightConstants
{
    Vector3 position = Vector3::Zero;
    float range = 0.0f;

    Vector3 direction = Vector3::Forward;
    float intensity = 0.0f;

    Color3 color = Color3::White;
    float innerConeAngle = 0.0f;

    float outerConeAngle = 0.0f;

    uint32 shadowIndex = UINT32_MAX;
    float padding[2];
};

struct ShadowConstants
{
    Matrix lightViewProj = Matrix::Identity;
    float bias = 0.0f;
    float padding[3];
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

    AmbientLightConstants ambientLight = {};
    DirectionalLightConstants directionalLight = {};
};

struct ShadowMapConstants
{
    Matrix world = Matrix::Identity;
    Matrix lightViewProj = Matrix::Identity;
};

struct SpotLightShadowConstants
{
    Matrix lightViewProj = Matrix::Identity;
    Vector4 altasUVScaleBias = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    float bias = 0.0f;
    float padding[3];
};

struct PointLightShadowConstants
{
    float range = 0.0f;
    float bias = 0.0f;
    float padding[2];
};

} // namespace URay::Render
