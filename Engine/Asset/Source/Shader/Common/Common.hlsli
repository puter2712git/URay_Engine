#ifndef __COMMON__
#define __COMMON__

float LinearViewDepth(float depth, float nearPlane, float farPlane)
{
    return (nearPlane * farPlane) /
        (farPlane - depth * (farPlane - nearPlane));
}

#endif
