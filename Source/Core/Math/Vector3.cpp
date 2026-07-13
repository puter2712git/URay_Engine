#include "Vector3.h"

namespace URay
{

Vector3 Vector3::Zero = Vector3();
Vector3 Vector3::One = Vector3(1.0f, 1.0f, 1.0f);

Vector3::Vector3(float inX, float inY, float inZ)
    : x(inX), y(inY), z(inZ)
{
}

} // namespace URay
