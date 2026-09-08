#include "Common.hlsli"
#include "VertexTypes.hlsli"

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

FragOut PSMain(VertexPNTOut input)
{
    FragOut output;

    output.outColor = textureImage.Sample(textureSampler, input.outUV) * obj.colorTint;

    return output;
}
