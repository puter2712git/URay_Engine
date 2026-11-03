using System.Runtime.InteropServices;

namespace URay;

[StructLayout(LayoutKind.Sequential)]
public struct Vector3
{
    public float x;
    public float y;
    public float z;

    public Vector3(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public static Vector3 Zero => new(0.0f, 0.0f, 0.0f);
    public static Vector3 One => new(1.0f, 1.0f, 1.0f);

    public static Vector3 Right => new(1.0f, 0.0f, 0.0f);
    public static Vector3 Forward => new(0.0f, 1.0f, 0.0f);
    public static Vector3 Up => new(0.0f, 0.0f, 1.0f);

    public float GetSquaredLength()
    {
        return x * x + y * y + z * z;
    }

    public float GetLength()
    {
        return MathF.Sqrt(GetSquaredLength());
    }

    public Vector3 GetNormalized()
    {
        float length = GetLength();

        if (length == 0.0f)
            return Zero;

        return this / length;
    }

    public static float Dot(Vector3 lhs, Vector3 rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    public static Vector3 Cross(Vector3 lhs, Vector3 rhs)
    {
        return new Vector3(
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x);
    }

    public static Vector3 operator +(Vector3 lhs, Vector3 rhs)
    {
        return new Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    public static Vector3 operator -(Vector3 lhs, Vector3 rhs)
    {
        return new Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    public static Vector3 operator *(Vector3 lhs, float scalar)
    {
        return new Vector3(lhs.x * scalar, lhs.y * scalar, lhs.z * scalar);
    }

    public static Vector3 operator /(Vector3 lhs, float scalar)
    {
        return new Vector3(lhs.x / scalar, lhs.y / scalar, lhs.z / scalar);
    }

    public static Vector3 operator *(float scalar, Vector3 rhs)
    {
        return rhs * scalar;
    }
}
