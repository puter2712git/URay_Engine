#include "CameraComponent.h"

#include "Core/Math/Math.h"

namespace URay
{

void CameraComponent::Update(float deltaTime)
{
    Component::Update(deltaTime);

    UpdateViewMatrix();
    UpdateProjMatrix();
}

void CameraComponent::UpdateViewMatrix()
{
    Vector3 forward = transform.TransformVectorNoScale(Vector3::Forward);
    Vector3 up = transform.TransformVectorNoScale(Vector3::Up);

    viewMatrix = Matrix::MakeView(transform.position, transform.position + forward, up);
}

void CameraComponent::UpdateProjMatrix()
{
    float fovRad = Math::DegToRad(fov);
    projMatrix = Matrix::MakePerspective(fovRad, 1.0f, near, far);
}

} // namespace URay
