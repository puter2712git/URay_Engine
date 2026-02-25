#include "Common/Math/Vector4.h"

namespace URay
{
	Vector4& Vector4::operator+=(const Vector4& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}

	Vector4& Vector4::operator-=(const Vector4& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	Vector4& Vector4::operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	Vector4 operator+(const Vector4& lhs, const Vector4& rhs)
	{
		return Vector4(lhs) += rhs;
	}

	Vector4 operator-(const Vector4& lhs, const Vector4& rhs)
	{
		return Vector4(lhs) -= rhs;
	}

	Vector4 operator*(const Vector4& lhs, float scalar)
	{
		return Vector4(lhs) *= scalar;
	}

	bool operator==(const Vector4& lhs, const Vector4& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
	}
}
