#include "Common.hlsli"
#include "VertexTypes.hlsli"

struct DecalConstants
{
    float4x4 invWorld;
};

[[vk::binding(0, 1)]] Texture2D<float4> decalTexture;
[[vk::binding(1, 1)]] SamplerState decalSampler;

[[vk::binding(0, 2)]] Texture2D<float> sceneDepthTexture;
[[vk::binding(1, 2)]] SamplerState sceneSampler;

[[vk::binding(0, 3)]] ConstantBuffer<DecalConstants> decal;

struct DecalVSOut
{
    float4 position : SV_POSITION;
};

DecalVSOut VSMain(VertexPNTIn input)
{
    DecalVSOut output;
    
    float4 worldPosition = mul(obj.world, float4(input.inPosition, 1.0));
    output.position = mul(frame.viewProj, worldPosition);
    
    return output;
}

FragOut PSMain(DecalVSOut input)
{
    FragOut output;
    
    float2 screenUV = input.position.xy / frame.renderTargetSize;
    
    float depth = sceneDepthTexture.Sample(sceneSampler, screenUV);

    if (depth >= 1.0)
        discard;

    float2 ndcXY = float2(
        screenUV.x * 2.0 - 1.0,
        1.0 - screenUV.y * 2.0);
    
    float4 clipPos = float4(ndcXY, depth, 1.0);
    
    float4 worldPos = mul(frame.invViewProj, clipPos);
    worldPos.xyz /= worldPos.w;
    
    float3 localPos = mul(decal.invWorld, float4(worldPos.xyz, 1.0)).xyz;
    
    if (any(abs(localPos) > float3(0.5, 0.5, 0.5)))
        discard;

    float2 decalUV = localPos.xz + 0.5;
    
    float4 decalColor = decalTexture.Sample(decalSampler, decalUV);
    
    output.outColor = decalColor;
    return output;
}
