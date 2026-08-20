#pragma once

#include "Engine/Component/Component.h"

#include "Core/Math/Extent2D.h"
#include "Core/Math/Matrix.h"

namespace URay
{

class CameraComponent : public Component
{
    URAY_CLASS(CameraComponent, Component)

public:
    void Update(float deltaTime) override;

    virtual const std::string& GetName() const override { return name; }

    Vector3 ScreenToWorld(const Vector3& screenPos) const;

    void SetViewportExtent(const Extent2D& extent);

    Matrix GetViewMatrix() const { return viewMatrix; }

    Matrix GetProjMatrix() const { return projMatrix; }

    float GetFOV() const { return fov; }

    float GetNearPlane() const { return near; }

    float GetFarPlane() const { return far; }

protected:
    std::string name = "Camera";

private:
    void UpdateViewMatrix();
    void UpdateProjMatrix();

private:
    float fov = 60.0f;

    float near = 0.1f;
    float far = 1000.0f;

    Extent2D viewportExtent = {};

    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;
};

} // namespace URay
