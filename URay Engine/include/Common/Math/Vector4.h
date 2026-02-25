#ifndef VECTOR4_H
#define VECTOR4_H

namespace URay
{
	struct Vector4
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;

		Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) : x(x), y(y), z(z), w(w) {}

		Vector4& operator+=(const Vector4&);
		Vector4& operator-=(const Vector4&);
		Vector4& operator*=(float scalar);
	};

	Vector4 operator+(const Vector4& lhs, const Vector4& rhs);
	Vector4 operator-(const Vector4& lhs, const Vector4& rhs);
	Vector4 operator*(const Vector4& lhs, float scalar);

	bool operator==(const Vector4& lhs, const Vector4& rhs);
}

#endif
