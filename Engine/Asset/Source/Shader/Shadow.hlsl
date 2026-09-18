#include "Common/ObjectConstants.hlsli"
#include "Common/FrameConstants.hlsli"
#include "Common/ShadowConstants.hlsli"

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
