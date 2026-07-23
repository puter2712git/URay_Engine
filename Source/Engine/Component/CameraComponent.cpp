#include "CameraComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Unit.h"

#include "Core/Math/Math.h"

namespace URay
{

void CameraComponent::Update(float deltaTime)
{
    Component::Update(deltaTime);

    UpdateViewMatrix();
    UpdateProjMatrix();
}

Vector3 CameraComponent::ScreenToWorld(const Vector3& screenPos) const
{
    float ndcX = (2.0f * screenPos.x) / 800.0f - 1.0f;
    float ndcY = 1.0f - (2.0f * screenPos.y) / 600.0f;

    Matrix invProjMatrix = GetProjMatrix().Inverse();
    Matrix invViewMatrix = GetViewMatrix().Inverse();

    const Vector4 ndc = Vector4(ndcX, ndcY, screenPos.z, 1.0f);
    const Vector4 clip = ndc * invProjMatrix;
    const Vector4 view = Vector4(clip.x / clip.w, clip.y / clip.w,
                                 clip.z / clip.w, 1.0f);
    const Vector4 world = view * invViewMatrix;
    const Vector3 worldPos = Vector3(world.x, world.y, world.z);

    return worldPos;
}

void CameraComponent::UpdateViewMatrix()
{
    Unit* owner = GetOwner();
    TransformComponent* transform = owner->GetTransform();

    Vector3 forward = transform->TransformVectorNoScale(Vector3::Forward);
    Vector3 up = transform->TransformVectorNoScale(Vector3::Up);

    viewMatrix = Matrix::MakeView(transform->GetPosition(), transform->GetPosition() + forward, up);
}

void CameraComponent::UpdateProjMatrix()
{
    float fovRad = Math::DegToRad(fov);
    projMatrix = Matrix::MakePerspective(fovRad, 1.0f, near, far);
}

} // namespace URay
