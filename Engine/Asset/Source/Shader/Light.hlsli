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

float3 EvaluateAmbient(AmbientLightConstants light)
{
    return light.color * light.intensity;
}

float3 EvaluateDirectional(DirectionalLightConstants light, float3 normal)
{
    float nDotL = saturate(dot(normalize(normal), -normalize(light.direction)));
    return light.color.rgb * light.intensity * nDotL;
}

float3 EvaluatePoint(PointLight light, float3 worldPos, float3 normal)
{
    float3 toLight = light.position - worldPos;
    float distanceToLight = length(toLight);
    
    if (distanceToLight >= light.radius)
        return 0.0.xxx;
    
    float3 L = toLight / max(distanceToLight, 1e-4);
    float3 attenuation = saturate(1.0 - distanceToLight / light.radius);
    attenuation *= attenuation;
    
    return light.color * light.intensity * saturate(dot(normalize(normal), L)) * attenuation;
}

#endif
