#ifndef VECTOR3_H
#define VECTOR3_H

namespace URay
{
	struct Vector3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

		Vector3& operator+=(const Vector3&);
		Vector3& operator-=(const Vector3&);
		Vector3& operator*=(float scalar);
	};

	Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
	Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
	Vector3 operator*(const Vector3& lhs, float scalar);

	bool operator==(const Vector3& lhs, const Vector3& rhs);
}

#endif
