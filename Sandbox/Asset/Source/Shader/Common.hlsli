#ifndef __COMMON__
#define __COMMON__

struct FrameConstants
{
    float4x4 view;
    float4x4 proj;
};
[[vk::binding(0, 0)]] ConstantBuffer<FrameConstants> frame;

struct ObjectConstants
{
    float4x4 world;
    float4 colorTint;
    uint objectId;
};
[[vk::push_constant]] ObjectConstants obj;

#endif
