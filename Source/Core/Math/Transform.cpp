#include "Transform.h"

#include "Core/Math/Matrix.h"

namespace URay
{

Transform Transform::Identity = Transform();

Transform::Transform(const Vector3& inPosition,
                     const Vector3& inRotation,
                     const Vector3& inScale)
    : position(inPosition), rotation(inRotation), scale(inScale)
{
}

Vector3 Transform::TransformPoint(const Vector3& point) const
{
    Matrix T = Matrix::MakeTranslation(position);
    Matrix R = Matrix::MakeRotation(rotation);
    Matrix S = Matrix::MakeScale(scale);

    Matrix worldMatrix = S * R * T;
    Vector4 vec4 = Vector4(point.x, point.y, point.z, 1.0f);

    Vector4 result = vec4 * worldMatrix;

    return Vector3(result.x, result.y, result.z);
}

Vector3 Transform::InvTransformPoint(const Vector3& worldPoint) const
{
    Matrix T = Matrix::MakeTranslation(position);
    Matrix R = Matrix::MakeRotation(rotation);
    Matrix S = Matrix::MakeScale(scale);

    Matrix invWorldMatrix = (S * R * T).Inverse();
    Vector4 vec4 = Vector4(worldPoint.x, worldPoint.y, worldPoint.z, 1.0f);

    Vector4 result = vec4 * invWorldMatrix;

    return Vector3(result.x, result.y, result.z);
}

Vector3 Transform::TransformVectorNoScale(const Vector3& vector) const
{
    Matrix rotationMatrix = Matrix::MakeRotation(rotation);
    Vector4 vec4 = Vector4(vector.x, vector.y, vector.z, 0.0f);

    Vector4 result = vec4 * rotationMatrix;

    return Vector3(result.x, result.y, result.z);
}

Vector3 Transform::InvTransformVectorNoScale(const Vector3& worldVector) const
{
    Matrix invRotMatrix = Matrix::MakeRotation(rotation).Inverse();
    Vector4 vec4 = Vector4(worldVector.x, worldVector.y, worldVector.z, 0.0f);

    Vector4 result = vec4 * invRotMatrix;

    return Vector3(result.x, result.y, result.z);
}

} // namespace URay
