#include "Common/Common.hlsli"
#include "Common/FrameConstants.hlsli"
#include "Common/ObjectConstants.hlsli"
#include "Common/ShadowConstants.hlsli"
#include "Common/VertexTypes.hlsli"

#ifndef URAY_SHADING_MODEL
#define URAY_SHADING_MODEL 1
#endif

[[vk::binding(0, 1)]] Texture2D<float4> diffuseColorTexture;
[[vk::binding(1, 1)]] SamplerState diffuseColorSampler;

float GetDirectionalShadowVisibility(float3 worldPosition, float3 normal)
{
    float4 lightClip = mul(directionalShadow.lightViewProj, float4(worldPosition, 1.0));
    float3 lightNdc = lightClip.xyz / max(lightClip.w, 1e-6);

    float2 shadowUV = float2(lightNdc.x * 0.5 + 0.5, 0.5 - lightNdc.y * 0.5);
    
    if (any(shadowUV < 0.0) || any(shadowUV > 1.0) ||
        lightNdc.z < 0.0 || lightNdc.z > 1.0)
    {
        return 1.0;
    }

    float depth = directionalShadowDepth.SampleLevel(directionalShadowSampler, shadowUV, 0);

    return lightNdc.z > depth + directionalShadow.bias ? 0.0 : 1.0;
}

float GetSpotShadowVisibility(SpotLight light, float3 worldPosition)
{
    if (light.shadowIndex == 0xffffffffu)
        return 1.0;
        
    SpotLightShadowConstants shadow = spotLightShadows[light.shadowIndex];
    
    float4 clip = mul(shadow.lightViewProj, float4(worldPosition, 1.0));
    float3 ndc = clip.xyz / max(clip.w, 1e-6);
    
    float2 localUV = float2(ndc.x * 0.5 + 0.5, 0.5 - ndc.y * 0.5);
    
    if (any(localUV < 0.0) || any(localUV > 1.0) ||
        ndc.z < 0.0 || ndc.z > 1.0)
    {
        return 1.0;
    }
    
    float2 atlasUV = localUV * shadow.atlasUVScaleBias.xy + shadow.atlasUVScaleBias.zw;
    
    float depth = shadowAtlas.SampleLevel(directionalShadowSampler, atlasUV, 0);
    
    return ndc.z > depth + shadow.bias ? 0.0 : 1.0;
}

float3 EvaluateLighting(float3 albedo, float3 worldPosition, float3 normal)
{
#if URAY_SHADING_MODEL == 1
    float3 lighting = EvaluateAmbient(frame.ambientLight);
    
    float shadowVisibility = GetDirectionalShadowVisibility(worldPosition, normal);
    lighting += EvaluateDirectional(frame.directionalLight, normal) * shadowVisibility;
        
    for (uint i = 0; i < 256; ++i)
    {
        lighting += EvaluatePoint(pointLights[i], worldPosition, normal);
    }
   
    for (uint i = 0; i < 256; ++i)
    {
        float spotShadow = GetSpotShadowVisibility(spotLights[i], worldPosition);
        lighting += EvaluateSpot(spotLights[i], worldPosition, normal) * spotShadow;
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
    output.outWorldPosition = worldPosition.xyz;
    output.outUV = input.inUV;
    output.outNormal = normalize(mul((float3x3) obj.world, input.inNormal));

    return output;
}

FragOut PSMain(VertexPNTOut input)
{
    FragOut output;
    
    float4 albedo = diffuseColorTexture.Sample(diffuseColorSampler, input.outUV) * obj.colorTint;
    float3 finalColor = EvaluateLighting(albedo.rgb, input.outWorldPosition, input.outNormal);
    
    output.outColor = float4(finalColor, albedo.a);

    return output;
}
