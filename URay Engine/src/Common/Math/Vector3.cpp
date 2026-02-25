#include "Common/Math/Vector3.h"

namespace URay
{
	Vector3& Vector3::operator+=(const Vector3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vector3& Vector3::operator-=(const Vector3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Vector3& Vector3::operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	Vector3 operator+(const Vector3& lhs, const Vector3& rhs)
	{
		return Vector3(lhs) += rhs;
	}

	Vector3 operator-(const Vector3& lhs, const Vector3& rhs)
	{
		return Vector3(lhs) -= rhs;
	}

	Vector3 operator*(const Vector3& lhs, float scalar)
	{
		return Vector3(lhs) *= scalar;
	}

	bool operator==(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}
}