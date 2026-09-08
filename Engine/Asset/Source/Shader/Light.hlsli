#ifndef __LIGHT__
#define __LIGHT__

struct AmbientLightConstants
{
    float intensity;
    float3 color;
};

struct DirectionalLightConstants
{
    float3 direction;
    float intensity;
    float4 color;
};

#endif
