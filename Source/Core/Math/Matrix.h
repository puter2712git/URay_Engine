#pragma once

#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>

#include <cstddef>

namespace URay
{

struct Quaternion;

struct Matrix
{
    union
    {
        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };

        float m[16];
        float elements[4][4];
    };

    Matrix();
    Matrix(float m00, float m01, float m02, float m03,
           float m10, float m11, float m12, float m13,
           float m20, float m21, float m22, float m23,
           float m30, float m31, float m32, float m33);

    static Matrix Identity;

    Matrix Transpose() const;
    Matrix Inverse() const;

    Vector3 TransformPoint(const Vector3& point) const;
    Vector3 TransformVector(const Vector3& vector) const;

    Vector3 GetPosition() const;
    Quaternion GetRotation() const;
    Vector3 GetEulerAngles() const;
    Vector3 GetScale() const;

    static Matrix MakeTranslation(const Vector3& position);
    static Matrix MakeRotation(const Quaternion& rotation);
    static Matrix MakeRotation(const Vector3& rotation);
    static Matrix MakeRotationX(float degree);
    static Matrix MakeRotationY(float degree);
    static Matrix MakeRotationZ(float degree);
    static Matrix MakeScale(const Vector3& scale);

    static Matrix MakeView(const Vector3& eye, const Vector3& target, const Vector3& up);
    static Matrix MakePerspective(float fov, float aspect, float near, float far);
    static Matrix MakeOrthogonal(float left, float right, float bottom, float top, float near, float far);

    Matrix operator*(const Matrix& rhs) const;
};

Vector4 operator*(const Vector4& lhs, const Matrix& rhs);

} // namespace URay
