#include "TransformComponent.h"

#include "Engine/Object/Class/Class.h"
#include "Engine/Scene/Unit.h"

namespace URay
{

URAY_REGISTER_CLASS(TransformComponent)
URAY_REGISTER_COMPONENT(TransformComponent)

void TransformComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::Vector3,
                                 .name = "Position",
                                 .offset = offsetof(TransformComponent, position),
                                 .size = sizeof(Vector3) });
    StaticClass()->AddProperty({ .type = PropertyType::Vector3,
                                 .name = "Rotation",
                                 .offset = offsetof(TransformComponent, rotation),
                                 .size = sizeof(Vector3) });
    StaticClass()->AddProperty({ .type = PropertyType::Vector3,
                                 .name = "Scale",
                                 .offset = offsetof(TransformComponent, scale),
                                 .size = sizeof(Vector3) });
}

void TransformComponent::Update(float deltaTime)
{
    Component::Update(deltaTime);

    if (isDirty)
    {
        UpdateWorldMatrix();
        isDirty = false;
    }
}

void TransformComponent::UpdateWorldMatrix()
{
    const Matrix T = Matrix::MakeTranslation(position);
    const Matrix R = Matrix::MakeRotation(rotation);
    const Matrix S = Matrix::MakeScale(scale);

    worldMatrix = S * R * T;

    GetOwner()->InvokeCallbacks();
}

Vector3 TransformComponent::TransformPoint(const Vector3& point) const
{
    Matrix T = Matrix::MakeTranslation(position);
    Matrix R = Matrix::MakeRotation(rotation);
    Matrix S = Matrix::MakeScale(scale);

    Matrix worldMatrix = S * R * T;
    Vector4 vec4 = Vector4(point.x, point.y, point.z, 1.0f);

    Vector4 result = vec4 * worldMatrix;

    return Vector3(result.x, result.y, result.z);
}

Vector3 TransformComponent::InvTransformPoint(const Vector3& worldPoint) const
{
    Matrix T = Matrix::MakeTranslation(position);
    Matrix R = Matrix::MakeRotation(rotation);
    Matrix S = Matrix::MakeScale(scale);

    Matrix invWorldMatrix = (S * R * T).Inverse();
    Vector4 vec4 = Vector4(worldPoint.x, worldPoint.y, worldPoint.z, 1.0f);

    Vector4 result = vec4 * invWorldMatrix;

    return Vector3(result.x, result.y, result.z);
}

Vector3 TransformComponent::TransformVector(const Vector3& vector) const
{
    Matrix rotationMatrix = Matrix::MakeRotation(rotation);
    Matrix scaleMatrix = Matrix::MakeScale(scale);
    Vector4 vec4 = Vector4(vector, 0.0f);

    Vector4 result = vec4 * scaleMatrix * rotationMatrix;

    return Vector3(result);
}

Vector3 TransformComponent::InvTransformVector(const Vector3& worldVector) const
{
    Matrix rotationMatrix = Matrix::MakeRotation(rotation);
    Matrix scaleMatrix = Matrix::MakeScale(scale);
    Matrix invMatrix = (scaleMatrix * rotationMatrix).Inverse();

    Vector4 vec4 = Vector4(worldVector, 0.0f);

    Vector4 result = vec4 * invMatrix;

    return Vector3(result);
}

Vector3 TransformComponent::TransformVectorNoScale(const Vector3& vector) const
{
    Matrix rotationMatrix = Matrix::MakeRotation(rotation);
    Vector4 vec4 = Vector4(vector.x, vector.y, vector.z, 0.0f);

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

} // namespace URay
