#include "Common.hlsli"
#include "VertexTypes.hlsli"

#ifndef URAY_SHADING_MODEL
#define URAY_SHADING_MODEL 1
#endif

[[vk::binding(0, 1)]] Texture2D<float4> diffuseColorTexture;
[[vk::binding(1, 1)]] SamplerState diffuseColorSampler;

float3 EvaluateLighting(float3 albedo, float3 worldPosition, float3 normal)
{
#if URAY_SHADING_MODEL == 1
    float3 lighting = EvaluateAmbient(frame.ambientLight);
    lighting += EvaluateDirectional(frame.directionalLight, normal);
        
    for (uint i = 0; i < 256; ++i)
    {
        lighting += EvaluatePoint(pointLights[i], worldPosition, normal);
    }
   
    for (uint i = 0; i < 256; ++i)
    {
        lighting += EvaluateSpot(spotLights[i], worldPosition, normal);
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
