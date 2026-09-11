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

struct PointLight
{
    float3 position;
    float radius;
    float intensity;
    float3 color;
};

[[vk::binding(1, 0)]]
StructuredBuffer<PointLight> pointLights;



#endif
