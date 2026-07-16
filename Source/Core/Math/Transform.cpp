#include "Transform.h"

#include "Core/Math/Matrix.h"

namespace URay
{

Transform Transform::Identity = {
    Vector3::Zero,
    Vector3::Zero,
    Vector3::One
};

Vector3 Transform::TransformVectorNoScale(const Vector3& vector) const
{
    Matrix rotationMatrix = Matrix::MakeRotation(rotation);
    Vector4 vec4 = Vector4(vector.x, vector.y, vector.z, 0.0f);

    Vector4 result = vec4 * rotationMatrix;

    return Vector3(result.x, result.y, result.z);
}

} // namespace URay
