#include "Common.hlsli"
#include "VertexTypes.hlsli"

[[vk::binding(0, 1)]] Texture2D<float4> textureImage;
[[vk::binding(1, 1)]] SamplerState textureSampler;

VertexPTCOut VSMain(VertexPTCIn input)
{
    VertexPTCOut output;

    output.outPosition = mul(frame.proj, mul(frame.view, mul(obj.world, float4(input.inPosition, 1.0))));
    output.outUV = input.inUV;
    output.fragColor = input.inColor;

    return output;
}

FragOut PSMain(VertexPTCOut input)
{
    FragOut output;

    float4 color = textureImage.Sample(textureSampler, input.outUV) * input.fragColor * obj.colorTint;

    float brightness = max(color.r, max(color.g, color.b));
    clip(brightness - 0.01);

    output.outColor = color;

    return output;
}
