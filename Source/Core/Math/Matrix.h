#pragma once

#include "Core/Math/Vector3.h"
#include "Core/Math/Vector4.h"

#include <cstddef>

namespace URay
{

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
    Matrix(
        float inM00, float inM01, float inM02, float inM03,
        float inM10, float inM11, float inM12, float inM13,
        float inM20, float inM21, float inM22, float inM23,
        float inM30, float inM31, float inM32, float inM33);

    static Matrix Identity;

    Matrix Transpose() const;

    Matrix Inverse() const;

    Vector3 TransformPoint(const Vector3& point) const;
    Vector3 TransformVector(const Vector3& vector) const;

    static Matrix MakeTranslation(const Vector3& position);

    static Matrix MakeRotation(const Vector3& rotation);
    static Matrix MakeRotationX(float degree);
    static Matrix MakeRotationY(float degree);
    static Matrix MakeRotationZ(float degree);

    static Matrix MakeScale(const Vector3& scale);

    static Matrix MakeView(const Vector3& eye, const Vector3& target, const Vector3& up);

    static Matrix MakePerspective(float fov, float aspect, float near, float far);

    Matrix operator*(const Matrix& rhs) const
    {
        Matrix ret = Matrix();

        for (size_t row = 0; row < 4; ++row)
        {
            for (size_t col = 0; col < 4; ++col)
            {
                for (size_t index = 0; index < 4; ++index)
                {
                    ret.elements[row][col] +=
                        elements[row][index] * rhs.elements[index][col];
                }
            }
        }

        return ret;
    }
};

Vector4 operator*(const Vector4& lhs, const Matrix& rhs);

} // namespace URay
