#include "TransformComponent.h"

#include <Engine/Component/Render/RenderComponent.h>
#include <Engine/Object/Class/Class.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

#include <vector>

namespace URay
{

URAY_REGISTER_CLASS(TransformComponent)
URAY_REGISTER_COMPONENT(TransformComponent)

void TransformComponent::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::Vector3,
          .name = "Position",
          .offset = offsetof(TransformComponent, position),
          .size = sizeof(Vector3) });
    StaticClass()->AddProperty(
        { .type = PropertyType::Rotation,
          .name = "Rotation",
          .offset = offsetof(TransformComponent, rotation),
          .size = sizeof(Vector3) });
    StaticClass()->AddProperty(
        { .type = PropertyType::Vector3,
          .name = "Scale",
          .offset = offsetof(TransformComponent, scale),
          .size = sizeof(Vector3) });
}

void TransformComponent::NotifyPropertyChanged(const Property& property)
{
    Super::NotifyPropertyChanged(property);

    SetDirty(true);
}

void TransformComponent::UpdateWorldMatrix()
{
    Unit* owner = GetOwner();
    if (!owner)
        return;

    TransformComponent* relativeTransform = FindRelativeTransform(owner->GetParent());

    const Matrix T = Matrix::MakeTranslation(position);
    const Matrix R = Matrix::MakeRotation(rotation);
    const Matrix S = Matrix::MakeScale(scale);

    worldMatrix = S * R * T;

    if (relativeTransform)
    {
        worldMatrix = worldMatrix * relativeTransform->GetWorldMatrix();
    }

    Scene* scene = owner->GetOwner();
    if (!scene)
        return;

    SceneSystem& sceneSystem = scene->GetSceneSystem();
    sceneSystem.onUnitWorldTransformUpdated.Emit(scene, owner);

    // TODO: Move these logics.
    for (const auto& component : GetOwner()->GetComponents())
    {
        if (RenderComponent* renderComp = Cast<RenderComponent>(component.get()))
        {
            renderComp->MarkDirty();
        }
    }
}

Vector3 TransformComponent::TransformPoint(const Vector3& localPoint) const
{
    return worldMatrix.TransformPoint(localPoint);
}

Vector3 TransformComponent::InvTransformPoint(const Vector3& worldPoint) const
{
    return worldMatrix.Inverse().TransformPoint(worldPoint);
}

Vector3 TransformComponent::TransformVector(const Vector3& localVector) const
{
    return worldMatrix.TransformVector(localVector);
}

Vector3 TransformComponent::InvTransformVector(const Vector3& worldVector) const
{
    return worldMatrix.Inverse().TransformVector(worldVector);
}

Vector3 TransformComponent::TransformVectorNoScale(const Vector3& localVector) const
{
    Matrix rotationMatrix = Matrix::MakeRotation(rotation);
    Vector4 vec4 = Vector4(localVector.x, localVector.y, localVector.z, 0.0f);

    Vector4 result = vec4 * rotationMatrix;

    return Vector3(result.x, result.y, result.z);
}

Vector3 TransformComponent::InvTransformVectorNoScale(const Vector3& worldVector) const
{
    Matrix invRotMatrix = Matrix::MakeRotation(rotation).Inverse();
    Vector4 vec4 = Vector4(worldVector.x, worldVector.y, worldVector.z, 0.0f);

    Vector4 result = vec4 * invRotMatrix;

    return Vector3(result.x, result.y, result.z);
}

Vector3 TransformComponent::GetForward() const
{
    Vector3 worldForward = Vector3::Forward;
    Vector3 forward = TransformVectorNoScale(worldForward);
    return forward;
}

Vector3 TransformComponent::GetRight() const
{
    Vector3 worldRight = Vector3::Right;
    Vector3 right = TransformVectorNoScale(worldRight);
    return right;
}

Vector3 TransformComponent::GetUp() const
{
    Vector3 worldUp = Vector3::Up;
    Vector3 up = TransformVectorNoScale(worldUp);
    return up;
}

void TransformComponent::SetDirty(bool dirty)
{
    if (isDirty == dirty)
        return;

    isDirty = dirty;

    if (!dirty)
        return;

    Unit* owner = GetOwner();
    if (!owner)
        return;

    std::vector<Unit*> pendingUnits(owner->GetChildren().begin(), owner->GetChildren().end());

    while (!pendingUnits.empty())
    {
        Unit* unit = pendingUnits.back();
        pendingUnits.pop_back();

        if (TransformComponent* transform = unit->GetTransform())
            transform->isDirty = true;

        const auto& children = unit->GetChildren();
        pendingUnits.insert(pendingUnits.end(), children.begin(), children.end());
    }
}

Vector3 TransformComponent::GetWorldPosition() const
{
    return worldMatrix.GetPosition();
}

void TransformComponent::SetWorldPosition(const Vector3& worldPosition)
{
    Unit* owner = GetOwner();

    TransformComponent* relativeTransform = owner ? FindRelativeTransform(owner->GetParent()) : nullptr;
    position = relativeTransform ? relativeTransform->InvTransformPoint(worldPosition) : worldPosition;

    SetDirty(true);
}

Quaternion TransformComponent::GetWorldRotation() const
{
    return worldMatrix.GetRotation();
}

void TransformComponent::SetWorldRotation(const Quaternion& worldRotation)
{
    Matrix localRotation = Matrix::MakeRotation(worldRotation);

    Unit* owner = GetOwner();

    TransformComponent* relativeTransform = owner ? FindRelativeTransform(owner->GetParent()) : nullptr;
    if (relativeTransform)
    {
        const Matrix parentWorldRotation = Matrix::MakeRotation(relativeTransform->GetWorldRotation());
        localRotation = localRotation * parentWorldRotation.Inverse();
    }

    SetLocalRotation(localRotation.GetRotation());
}

Vector3 TransformComponent::GetWorldForward() const
{
    return worldMatrix.TransformVector(Vector3::Forward).GetNormalized();
}

Vector3 TransformComponent::GetWorldRight() const
{
    return worldMatrix.TransformVector(Vector3::Right).GetNormalized();
}

Vector3 TransformComponent::GetWorldUp() const
{
    return worldMatrix.TransformVector(Vector3::Up).GetNormalized();
}

void TransformComponent::OnInitialize()
{
    isDirty = true;
}

void TransformComponent::OnUpdate(float deltaTime)
{
    if (isDirty)
    {
        UpdateWorldMatrix();
        isDirty = false;
    }
}

TransformComponent* TransformComponent::FindRelativeTransform(Unit* parentUnit) const
{
    if (!parentUnit)
        return nullptr;

    TransformComponent* parentTransform = parentUnit->GetTransform();
    if (parentTransform)
        return parentTransform;

    return FindRelativeTransform(parentUnit->GetParent());
}

} // namespace URay
