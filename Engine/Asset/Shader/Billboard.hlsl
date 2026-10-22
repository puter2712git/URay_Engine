#include "Common/Common.hlsli"
#include "Common/FrameConstants.hlsli"
#include "Common/ObjectConstants.hlsli"
#include "Common/VertexTypes.hlsli"

#ifndef URAY_SELECTION_MASK
#define URAY_SELECTION_MASK 0
#endif

[[vk::binding(0, 1)]] Texture2D<float4> textureImage;
[[vk::binding(1, 1)]] SamplerState textureSampler;

VertexPNTOut VSMain(VertexPNTIn input)
{
    VertexPNTOut output;

    // The default quad lies on the local XZ plane. Use only the object's
    // translation as the billboard center, then expand it in view-space XY.
    float4 centerWS = mul(obj.world, float4(0.0, 0.0, 0.0, 1.0));
    float4 centerVS = mul(frame.view, centerWS);
    float2 offset = input.inPosition.xz;

    output.outPosition = mul(frame.proj, centerVS + float4(offset, 0.0, 0.0));
    output.outUV = input.inUV;

    return output;
}

struct BillboardFragOut
{
    [[vk::location(0)]] float4 outColor : SV_Target;
#if !URAY_SELECTION_MASK
    [[vk::location(1)]] uint outObjectId : SV_Target1;
#endif
};

BillboardFragOut PSMain(VertexPNTOut input)
{
    BillboardFragOut output;

    float4 color = textureImage.Sample(textureSampler, input.outUV) * obj.colorTint;
    
    clip(color.a - 0.01f);
    
#if URAY_SELECTION_MASK
    output.outColor = float4(1, 1, 1, 1);
#else
    output.outColor = color;
    output.outObjectId = obj.objectId;
#endif

    return output;
}
