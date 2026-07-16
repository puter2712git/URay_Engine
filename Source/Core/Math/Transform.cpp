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

Vector3 Transform::TransformVectorNoScale(const Vector3& vector) const
{
    Matrix rotationMatrix = Matrix::MakeRotation(rotation);
    Vector4 vec4 = Vector4(vector.x, vector.y, vector.z, 0.0f);

    Vector4 result = vec4 * rotationMatrix;

    return Vector3(result.x, result.y, result.z);
}

} // namespace URay
