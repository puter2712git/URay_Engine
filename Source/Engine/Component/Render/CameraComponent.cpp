#include "CameraComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"

#include "Core/Math/Math.h"
#include "Core/Type/Types.h"

#include "Render/Scene/Object/ViewObject.h"

namespace URay
{

URAY_REGISTER_CLASS(CameraComponent)
URAY_REGISTER_COMPONENT(CameraComponent)

void CameraComponent::RegisterClass()
{
    Super::RegisterClass();
}

void CameraComponent::Update(float deltaTime)
{
    Super::Update(deltaTime);

    UpdateViewMatrix();
    UpdateProjMatrix();
}

Render::RenderObject* CameraComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    TransformComponent* transform = owner->GetTransform();
    Matrix worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;

    renderObject = new Render::ViewObject(worldMatrix, viewMatrix, projMatrix);
    return renderObject;
}

Vector3 CameraComponent::ScreenToWorld(const Vector3& screenPos) const
{
    uint32 width = viewportExtent.width;
    uint32 height = viewportExtent.height;

    float ndcX = (2.0f * screenPos.x) / width - 1.0f;
    float ndcY = 1.0f - (2.0f * screenPos.y) / height;

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

void CameraComponent::SetViewportExtent(const Extent2D& extent)
{
    if (extent.width == 0 || extent.height == 0)
        return;

    if (viewportExtent.width == extent.width &&
        viewportExtent.height == extent.height)
    {
        return;
    }

    viewportExtent = extent;
    UpdateProjMatrix();
}

void CameraComponent::UpdateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return;

    TransformComponent* transform = owner->GetTransform();

    Render::ViewObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.viewMatrix = viewMatrix;
    state.projMatrix = projMatrix;

    Render::ViewObject* viewObject = static_cast<Render::ViewObject*>(renderObject);
    viewObject->Update(state);
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
    uint32 width = viewportExtent.width;
    uint32 height = viewportExtent.height;

    if (width == 0 || height == 0)
        return;

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float fovRad = Math::DegToRad(fov);

    projMatrix = Matrix::MakePerspective(fovRad, aspectRatio, near, far);
}

} // namespace URay
