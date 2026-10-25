#pragma once

#include <Core/Math/Extent2D.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>

namespace URay
{

struct EditorView
{
    Vector3 position = Vector3::Zero;
    Vector3 rotation = Vector3::Zero;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float fov = 45.0f;

    Vector3 GetForward() const;
    Vector3 GetUp() const;

    Matrix GetViewMatrix() const;
    Matrix GetProjMatrix(const Extent2D& extent) const;

    void ScreenPointToRay(const Vector2& point, const Extent2D& extent, Vector3& outOrigin, Vector3& outDirection) const;
};

} // namespace URay
