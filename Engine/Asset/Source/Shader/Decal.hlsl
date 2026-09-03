#include "Common.hlsli"
#include "VertexTypes.hlsli"

struct DecalConstants
{
    float4x4 invWorld;
    float3 extent;
    float padding;
};

[[vk::binding(0, 1)]] Texture2D<float4> decalTexture;
[[vk::binding(1, 1)]] SamplerState decalSampler;

[[vk::binding(0, 2)]] ConstantBuffer<DecalConstants> decal;

struct DecalVSOut
{
    float4 position : SV_POSITION;
    float3 worldPosition : TEXCOORD0;
};

DecalVSOut VSMain(VertexPNTIn input)
{
    DecalVSOut output;
    
    float4 worldPosition = mul(obj.world, float4(input.inPosition, 1.0));
    output.worldPosition = worldPosition.xyz;
    output.position = mul(frame.viewProj, worldPosition);
    
    return output;
}

FragOut PSMain(DecalVSOut input)
{
    FragOut output;
        
    float3 localPos = mul(decal.invWorld, float4(input.worldPosition, 1.0)).xyz;

    if (any(abs(localPos) > decal.extent))
        discard;
    
    float2 uv = localPos.xz / (decal.extent.xz * 2.0) + 0.5;
    
    output.outColor = decalTexture.Sample(decalSampler, uv);
    
    return output;
}
