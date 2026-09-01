#ifndef __COMMON__
#define __COMMON__

struct FrameConstants
{
    float4x4 view;
    float4x4 proj;
    float nearPlane;
    float farPlane;
};
[[vk::binding(0, 0)]] ConstantBuffer<FrameConstants> frame;

struct ObjectConstants
{
    float4x4 world;
    float4 colorTint;
    uint objectId;
};
[[vk::push_constant]] ObjectConstants obj;

float LinearViewDepth(float depth, float nearPlane, float farPlane)
{
    return (nearPlane * farPlane) /
        (farPlane - depth * (farPlane - nearPlane));

}

#endif
