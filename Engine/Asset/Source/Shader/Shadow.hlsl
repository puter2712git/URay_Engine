#include "Common/FrameConstants.hlsli"

struct ShadowMapConstants
{
    float4x4 world;
    float4x4 lightViewProj;
};
[[vk::push_constant]] ShadowMapConstants shadowMap;

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
    
    const float4 worldPosition = mul(shadowMap.world, float4(input.inPosition, 1.0));

    output.outPosition = mul(shadowMap.lightViewProj, worldPosition);
    
    return output;
}

void PSMain(ShadowVertexOut input)
{
}
