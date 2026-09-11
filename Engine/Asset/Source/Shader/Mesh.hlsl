#include "Common.hlsli"
#include "VertexTypes.hlsli"

#ifndef URAY_SHADING_MODEL
#define URAY_SHADING_MODEL 1
#endif

[[vk::binding(0, 1)]] Texture2D<float4> diffuseColorTexture;
[[vk::binding(1, 1)]] SamplerState diffuseColorSampler;

float4 EvaluateSurfaceColor(VertexPNTOut input)
{
    return diffuseColorTexture.Sample(diffuseColorSampler, input.outUV) * obj.colorTint;
}

float3 EvaluateLighting(float3 albedo, float3 worldPosition, float3 normal)
{
#if URAY_SHADING_MODEL == 1
    float3 lighting = frame.ambientLight.color * frame.ambientLight.intensity;
    
    float nDotL = saturate(dot(normalize(normal), -normalize(frame.directionalLight.direction)));
    lighting += frame.directionalLight.color.rgb * frame.directionalLight.intensity * nDotL;
    
    for (uint i = 0; i < 256; ++i)
    {
        PointLight light = pointLights[i];
        float3 toLight = light.position - worldPosition;
        float distanceToLight = length(toLight);

        if (distanceToLight >= light.radius)
            continue;

        float3 L = toLight / max(distanceToLight, 1e-4);
        float3 attenuation = saturate(1.0 - distanceToLight / light.radius);
        attenuation *= attenuation;
        
        lighting += light.color * light.intensity
            * saturate(dot(normalize(normal), L))
            * attenuation;
    }
    
    return albedo * lighting;
#else
    return albedo;
#endif
}

VertexPNTOut VSMain(VertexPNTIn input)
{
    VertexPNTOut output;
    
    float4 worldPosition = mul(obj.world, float4(input.inPosition, 1.0));

    output.outPosition = mul(frame.viewProj, worldPosition);
    output.outWorldPosition = worldPosition;
    output.outUV = input.inUV;
    output.outNormal = normalize(mul((float3x3) obj.world, input.inNormal));

    return output;
}

FragOut PSMain(VertexPNTOut input)
{
    FragOut output;
    
    float4 albedo = EvaluateSurfaceColor(input);
    float3 finalColor = EvaluateLighting(albedo.rgb, input.outWorldPosition, input.outNormal);
    
    output.outColor = float4(finalColor, albedo.a);

    return output;
}
