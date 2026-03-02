#ifndef VECTOR2_H
#define VECTOR2_H

namespace URay
{
	struct Vector2
	{
		float x = 0.0f;
		float y = 0.0f;

		Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

		Vector2& operator+=(const Vector2&);
		Vector2& operator-=(const Vector2&);
		Vector2& operator*=(float scalar);
	};

	Vector2 operator+(const Vector2& lhs, const Vector2& rhs);
	Vector2 operator-(const Vector2& lhs, const Vector2& rhs);
	Vector2 operator*(const Vector2& lhs, float scalar);

	bool operator==(const Vector2& lhs, const Vector2& rhs);
}

#endif
