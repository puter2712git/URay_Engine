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

float3 EvaluateLighting(float3 albedo, float3 normal)
{
#if URAY_SHADING_MODEL == 1
    float nDotL = saturate(dot(normalize(normal), -normalize(frame.directionalLight.direction)));
    float3 ambient = frame.ambientLight.color * frame.ambientLight.intensity;
    float3 directLight = frame.directionalLight.color.rgb * frame.directionalLight.intensity * nDotL;

    return albedo * (ambient + directLight);
#else
    return albedo;
#endif
}

VertexPNTOut VSMain(VertexPNTIn input)
{
    VertexPNTOut output;
    
    float4 worldPosition = mul(obj.world, float4(input.inPosition, 1.0));

    output.outPosition = mul(frame.viewProj, worldPosition);
    output.outUV = input.inUV;
    output.outNormal = normalize(mul((float3x3) obj.world, input.inNormal));

    return output;
}

FragOut PSMain(VertexPNTOut input)
{
    FragOut output;
    
    float4 albedo = EvaluateSurfaceColor(input);
    float3 finalColor = EvaluateLighting(albedo.rgb, input.outNormal);
    
    output.outColor = float4(finalColor, albedo.a);

    return output;
}
