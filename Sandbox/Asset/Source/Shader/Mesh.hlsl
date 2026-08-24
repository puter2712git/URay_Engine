#include "Common.hlsli"
#include "VertexTypes.hlsli"

[[vk::binding(0, 1)]] Texture2D<float4> diffuseColorTexture;
[[vk::binding(1, 1)]] SamplerState diffuseColorSampler;

VertexPNTOut VSMain(VertexPNTIn input)
{
    VertexPNTOut output;
    
    float4 worldPosition = mul(obj.world, float4(input.inPosition, 1.0));

    output.outPosition = mul(frame.proj, mul(frame.view, worldPosition));
    output.outUV = input.inUV;
    output.outNormal = normalize(mul((float3x3) obj.world, input.inNormal));

    return output;
}

FragOut PSMain(VertexPNTOut input)
{
    FragOut output;

    float4 baseColor = diffuseColorTexture.Sample(diffuseColorSampler, input.outUV);
    
    output.outColor = baseColor * obj.colorTint;

    return output;
}
