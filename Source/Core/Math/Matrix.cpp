#include "Matrix.h"

#include "Core/Math/Math.h"

#include <cmath>

namespace URay
{

Matrix Matrix::Identity = Matrix(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);

Matrix::Matrix()
    : m00(0.0f), m01(0.0f), m02(0.0f), m03(0.0f),
      m10(0.0f), m11(0.0f), m12(0.0f), m13(0.0f),
      m20(0.0f), m21(0.0f), m22(0.0f), m23(0.0f),
      m30(0.0f), m31(0.0f), m32(0.0f), m33(0.0f)
{
}

Matrix::Matrix(
    float inM00, float inM01, float inM02, float inM03,
    float inM10, float inM11, float inM12, float inM13,
    float inM20, float inM21, float inM22, float inM23,
    float inM30, float inM31, float inM32, float inM33)
    : m00(inM00), m01(inM01), m02(inM02), m03(inM03),
      m10(inM10), m11(inM11), m12(inM12), m13(inM13),
      m20(inM20), m21(inM21), m22(inM22), m23(inM23),
      m30(inM30), m31(inM31), m32(inM32), m33(inM33)
{
}

Matrix Matrix::Transpose() const
{
    Matrix ret;

    for (size_t row = 0; row < 4; ++row)
    {
        for (size_t col = 0; col < 4; ++col)
        {
            ret.elements[row][col] = elements[col][row];
        }
    }

    return ret;
}

Matrix Matrix::Inverse() const
{
    float src[12];
    float dst[16];

    // 중간 계산을 위한 소행렬식(Minor) 쌍 생성
    src[0] = m[10] * m[15] - m[11] * m[14];
    src[1] = m[9] * m[15] - m[11] * m[13];
    src[2] = m[9] * m[14] - m[10] * m[13];
    src[3] = m[8] * m[15] - m[11] * m[12];
    src[4] = m[8] * m[14] - m[10] * m[12];
    src[5] = m[8] * m[13] - m[9] * m[12];
    src[6] = m[2] * m[7] - m[3] * m[6];
    src[7] = m[1] * m[7] - m[3] * m[5];
    src[8] = m[1] * m[6] - m[2] * m[5];
    src[9] = m[0] * m[7] - m[3] * m[4];
    src[10] = m[0] * m[6] - m[2] * m[4];
    src[11] = m[0] * m[5] - m[1] * m[4];

    // 수반 행렬(Adjugate Matrix) 요소 계산
    dst[0] = m[5] * src[0] - m[6] * src[1] + m[7] * src[2];
    dst[1] = -m[1] * src[0] + m[2] * src[1] - m[3] * src[2];
    dst[2] = m[13] * src[6] - m[14] * src[7] + m[15] * src[8];
    dst[3] = -m[9] * src[6] + m[10] * src[7] - m[11] * src[8];

    dst[4] = -m[4] * src[0] + m[6] * src[3] - m[7] * src[4];
    dst[5] = m[0] * src[0] - m[2] * src[3] + m[3] * src[4];
    dst[6] = -m[12] * src[6] + m[14] * src[9] - m[15] * src[10];
    dst[7] = m[8] * src[6] - m[10] * src[9] + m[11] * src[10];

    dst[8] = m[4] * src[1] - m[5] * src[3] + m[7] * src[5];
    dst[9] = -m[0] * src[1] + m[1] * src[3] - m[3] * src[5];
    dst[10] = m[12] * src[7] - m[13] * src[9] + m[15] * src[11];
    dst[11] = -m[8] * src[7] + m[9] * src[9] - m[11] * src[11];

    dst[12] = -m[4] * src[2] + m[5] * src[4] - m[6] * src[5];
    dst[13] = m[0] * src[2] - m[1] * src[4] + m[2] * src[5];
    dst[14] = -m[12] * src[8] + m[13] * src[10] - m[14] * src[11];
    dst[15] = m[8] * src[8] - m[9] * src[10] + m[10] * src[11];

    // 행렬식(Determinant) 계산
    float det = m[0] * dst[0] + m[1] * dst[4] + m[2] * dst[8] + m[3] * dst[12];

    // 결과 행렬 생성
    Matrix result;

    // 행렬식이 0에 너무 가까우면 역행렬이 존재하지 않으므로 에러 처리나 기본 행렬 반환
    if (std::abs(det) < 1e-9f)
    {
        // 프로젝트 규칙에 맞게 처리하세요 (예: Identity 반환 혹은 에러 던지기)
        return Matrix();
    }

    // 행렬식의 역수를 각 요소에 곱해줌
    float invDet = 1.0f / det;
    for (int i = 0; i < 16; i++)
    {
        result.m[i] = dst[i] * invDet;
    }

    return result;
}

Vector3 Matrix::TransformPoint(const Vector3& point) const
{
    const Vector4 result = Vector4(point.x, point.y, point.z, 1.0f) * *this;
    return Vector3(result.x, result.y, result.z);
}

Vector3 Matrix::TransformVector(const Vector3& vector) const
{
    const Vector4 result = Vector4(vector.x, vector.y, vector.z, 0.0f) * *this;
    return Vector3(result.x, result.y, result.z);
}

Matrix Matrix::MakeTranslation(const Vector3& position)
{
    Matrix mat = Matrix::Identity;
    mat.elements[3][0] = position.x;
    mat.elements[3][1] = position.y;
    mat.elements[3][2] = position.z;
    mat.elements[3][3] = 1.0f;

    return mat;
}

Matrix Matrix::MakeRotation(const Vector3& rotation)
{
    Matrix rotX = MakeRotationX(rotation.x);
    Matrix rotY = MakeRotationY(rotation.y);
    Matrix rotZ = MakeRotationZ(rotation.z);

    Matrix mat = rotX * rotY * rotZ;

    return mat;
}

Matrix Matrix::MakeRotationX(float degree)
{
    Matrix mat = Matrix::Identity;
    float rad = Math::DegToRad(degree);

    mat.elements[1][1] = std::cos(rad);
    mat.elements[1][2] = std::sin(rad);
    mat.elements[2][1] = -std::sin(rad);
    mat.elements[2][2] = std::cos(rad);

    return mat;
}

Matrix Matrix::MakeRotationY(float degree)
{
    Matrix mat = Matrix::Identity;
    float rad = Math::DegToRad(degree);

    mat.elements[0][0] = std::cos(rad);
    mat.elements[0][2] = -std::sin(rad);
    mat.elements[2][0] = std::sin(rad);
    mat.elements[2][2] = std::cos(rad);

    return mat;
}

Matrix Matrix::MakeRotationZ(float degree)
{
    Matrix mat = Matrix::Identity;
    float rad = Math::DegToRad(degree);

    mat.elements[0][0] = std::cos(rad);
    mat.elements[0][1] = std::sin(rad);
    mat.elements[1][0] = -std::sin(rad);
    mat.elements[1][1] = std::cos(rad);

    return mat;
}

Matrix Matrix::MakeScale(const Vector3& scale)
{
    Matrix mat = Matrix::Identity;
    mat.elements[0][0] = scale.x;
    mat.elements[1][1] = scale.y;
    mat.elements[2][2] = scale.z;

    return mat;
}

Matrix Matrix::MakeView(const Vector3& eye, const Vector3& target, const Vector3& up)
{
    Vector3 forward = (target - eye).GetNormalized();
    Vector3 right = Vector3::Cross(forward, up).GetNormalized();
    Vector3 cameraUp = Vector3::Cross(right, forward);

    Matrix ret = Matrix(
        right.x, cameraUp.x, forward.x, 0.0f,
        right.y, cameraUp.y, forward.y, 0.0f,
        right.z, cameraUp.z, forward.z, 0.0f,
        -Vector3::Dot(right, eye), -Vector3::Dot(cameraUp, eye), -Vector3::Dot(forward, eye), 1.0f);

    return ret;
}

Matrix Matrix::MakePerspective(float fov, float aspect, float near, float far)
{
    const float tanHalfFov = std::tan(fov * 0.5f);

    const float A = far / (far - near);
    const float B = -(far * near) / (far - near);

    Matrix ret = Matrix(
        1 / (aspect * tanHalfFov), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, A, 1.0f,
        0.0f, 0.0f, B, 0.0f);

    return ret;
}

Vector4 operator*(const Vector4& lhs, const Matrix& rhs)
{
    return Vector4(
        lhs.x * rhs.m00 + lhs.y * rhs.m10 + lhs.z * rhs.m20 + lhs.w * rhs.m30,
        lhs.x * rhs.m01 + lhs.y * rhs.m11 + lhs.z * rhs.m21 + lhs.w * rhs.m31,
        lhs.x * rhs.m02 + lhs.y * rhs.m12 + lhs.z * rhs.m22 + lhs.w * rhs.m32,
        lhs.x * rhs.m03 + lhs.y * rhs.m13 + lhs.z * rhs.m23 + lhs.w * rhs.m33);
}

} // namespace URay
