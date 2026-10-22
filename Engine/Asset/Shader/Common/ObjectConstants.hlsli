#ifndef __OBJECT_CONSTANTS__
#define __OBJECT_CONSTANTS__

struct ObjectConstants
{
    float4x4 world;
    float4 colorTint;
    uint objectId;
};
[[vk::push_constant]] ObjectConstants obj;

#endif
