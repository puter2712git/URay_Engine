#include "Common/Common.hlsli"
#include "Common/FrameConstants.hlsli"
#include "Common/ObjectConstants.hlsli"
#include "Common/VertexTypes.hlsli"

#ifndef URAY_SELECTION_MASK
#define URAY_SELECTION_MASK 0
#endif

struct SpriteFragOut
{
    [[vk::location(0)]] float4 outColor : SV_Target0;
    
#if !URAY_SELECTION_MASK
    [[vk::location(1)]] uint outObjectId : SV_Target1;
#endif
};

[[vk::binding(0, 1)]] Texture2D<float4> textureImage;
[[vk::binding(1, 1)]] SamplerState textureSampler;

VertexPNTOut VSMain(VertexPNTIn input)
{
    VertexPNTOut output;

    output.outPosition = mul(frame.proj, mul(frame.view, mul(obj.world, float4(input.inPosition, 1.0))));
    output.outUV = input.inUV;

    return output;
}

SpriteFragOut PSMain(VertexPNTOut input)
{
    SpriteFragOut output;
    
    float4 color = textureImage.Sample(textureSampler, input.outUV) * obj.colorTint;

    if (color.a <= 0.0f)
    {
        discard;
    }
    
#if URAY_SELECTION_MASK
    output.outColor = float4(1.0, 1.0, 1.0, 1.0);
#else
    output.outColor = color;
    output.outObjectId = obj.objectId;
#endif

    return output;
}
