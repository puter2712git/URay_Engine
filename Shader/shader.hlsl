#include "Common.hlsli"
#include "VertexTypes.hlsli"

[[vk::binding(0, 1)]] Texture2D<float4> textureImage;
[[vk::binding(1, 1)]] SamplerState textureSampler;

VertexPNTOut VSMain(VertexPNTIn input)
{
    VertexPNTOut output;

    output.outPosition = mul(frame.proj, mul(frame.view, mul(obj.world, float4(input.inPosition, 1.0))));
    output.outUV = input.inUV;

    return output;
}

FragOut PSMain(VertexPNTOut input)
{
    FragOut output;

    output.outColor = textureImage.Sample(textureSampler, input.outUV) * obj.colorTint;

    return output;
}
