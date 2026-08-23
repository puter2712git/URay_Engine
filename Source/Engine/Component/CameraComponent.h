#pragma once

#include "Engine/Component/Component.h"
#include "Engine/Component/IRenderable.h"

#include "Core/Math/Extent2D.h"
#include "Core/Math/Matrix.h"

namespace URay
{

namespace RHI
{
class ViewObject;
}

class CameraComponent : public Component, public IRenderable
{
    URAY_CLASS(CameraComponent, Component)

public:
    void Update(float deltaTime) override;

    void OnAttached() override;
    void OnDetached() override;

    RHI::RenderObject* CreateRenderObject() override;

    Vector3 ScreenToWorld(const Vector3& screenPos) const;

    void SetViewportExtent(const Extent2D& extent);

    Matrix GetViewMatrix() const { return viewMatrix; }
    Matrix GetProjMatrix() const { return projMatrix; }
    float GetFOV() const { return fov; }
    float GetNearPlane() const { return near; }
    float GetFarPlane() const { return far; }

private:
    void UpdateViewMatrix();
    void UpdateProjMatrix();

private:
    RHI::ViewObject* renderObject = nullptr;

    float fov = 60.0f;

    float near = 0.1f;
    float far = 1000.0f;

    Extent2D viewportExtent = {};

    Matrix viewMatrix = Matrix::Identity;
    Matrix projMatrix = Matrix::Identity;
};

} // namespace URay
