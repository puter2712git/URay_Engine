#include "Common.hlsli"
#include "VertexTypes.hlsli"

[[vk::binding(0, 1)]] Texture2D<float4> diffuseColorTexture;
[[vk::binding(1, 1)]] SamplerState diffuseColorSampler;

VertexPNTOut VSMain(VertexPNTIn input)
{
    VertexPNTOut output;
    
    float4 worldPosition = mul(obj.world, float4(input.inPosition, 1.0));

    output.outPosition = mul(frame.viewProj, worldPosition);
    output.outUV = input.inUV;
    output.outNormal = normalize(mul((float3x3) obj.world, input.inNormal));

    return output;
}

FragOut PSMain(VertexPNTOut input)
{
    FragOut output;

    float4 baseColor = diffuseColorTexture.Sample(diffuseColorSampler, input.outUV);
    float3 normal = normalize(input.outNormal);
    float3 lightToSurface = normalize(frame.lightDirection);
    
    float nDotL = saturate(dot(normal, -lightToSurface));
    
    float3 ambient = 0.05.xxx;
    float3 directLight = frame.lightColor.rgb * frame.lightIntensity * nDotL;
    
    output.outColor.rgb = baseColor.rgb * obj.colorTint.rgb * (ambient + directLight);
    output.outColor.a = baseColor.a * obj.colorTint.a;

    return output;
}
