#include "Common/ObjectConstants.hlsli"

struct ShadowConstants
{
    float4x4 lightViewProj;
};
[[vk::binding(2, 0)]] ConstantBuffer<ShadowConstants> shadow;

struct ShadowVertexIn
{
    [[vk::location(0)]] float3 inPosition : POSITION;
};

struct ShadowVertexOut
{
    float4 outPosition : SV_Position;
};

ShadowVertexOut VSMain(ShadowVertexIn input)
{
    ShadowVertexOut output;
    
    const float4 worldPosition = mul(obj.world, float4(input.inPosition, 1.0));

    output.outPosition = mul(shadow.lightViewProj, worldPosition);
    
    return output;
}

void PSMain(ShadowVertexOut input)
{
}
