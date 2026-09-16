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

struct SpotLight
{
    float3 position;
    float range;
    float3 direction;
    float intensity;
    float3 color;
    float innerConeAngle;
    float outerConeAngle;
    float3 padding;
};

[[vk::binding(1, 0)]]
StructuredBuffer<PointLight> pointLights;
[[vk::binding(2, 0)]]
StructuredBuffer<SpotLight> spotLights;

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

float3 EvaluateSpot(SpotLight light, float3 worldPos, float3 normal)
{
    float3 toLight = light.position - worldPos;
    float distanceToLight = length(toLight);

    if (distanceToLight >= light.range)
        return 0.0.xxx;

    float3 L = toLight / max(distanceToLight, 1e-4);
    
    float coneCos = dot(normalize(light.direction), -L);
    float innerCos = cos(light.innerConeAngle);
    float outerCos = cos(light.outerConeAngle);

    float coneAttenuation = saturate((coneCos - outerCos) / max(innerCos - outerCos, 1e-4));
    float rangeAttenuation = saturate(1.0 - distanceToLight / light.range);
    rangeAttenuation *= rangeAttenuation;

    float nDotL = saturate(dot(normalize(normal), L));
    
    return light.color * light.intensity * nDotL * rangeAttenuation * coneAttenuation;
}

#endif
