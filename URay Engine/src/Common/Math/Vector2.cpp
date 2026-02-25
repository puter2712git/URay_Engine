#include "Common/Math/Vector2.h"

namespace URay
{
	Vector2& Vector2::operator+=(const Vector2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vector2& Vector2::operator-=(const Vector2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vector2& Vector2::operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
	{
		return Vector2(lhs) += rhs;
	}

	Vector2 operator-(const Vector2& lhs, const Vector2& rhs)
	{
		return Vector2(lhs) -= rhs;
	}

	Vector2 operator*(const Vector2& lhs, float scalar)
	{
		return Vector2(lhs) *= scalar;
	}

	bool operator==(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}
}