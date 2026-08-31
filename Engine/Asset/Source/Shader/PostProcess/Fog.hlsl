struct FogConstants
{
    float4 fogColor;
    float fogStart;
    float fogEnd;
    float density;
    uint enabled;
};

[[vk::binding(0, 2)]]
Texture2D<float4> sceneColorTexture;

[[vk::binding(1, 2)]]
Texture2D<float> sceneDepthTexture;

[[vk::binding(2, 2)]]
SamplerState sceneSampler;

[[vk::binding(3, 2)]]
ConstantBuffer<FogConstants> fog;

struct VSOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOut VSMain(uint vertexId : SV_VertexID)
{
    float2 positions[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0, 3.0),
        float2(3.0, -1.0)
    };
    
    float2 uvs[3] =
    {
        float2(0.0, 1.0),
        float2(0.0, -1.0),
        float2(2.0, 1.0)
    };
    
    VSOut output;
    output.position = float4(positions[vertexId], 0.0, 1.0);
    output.uv = uvs[vertexId];
    return output;

}

float4 PSMain(VSOut input) : SV_TARGET
{
    float4 sceneColor = sceneColorTexture.Sample(sceneSampler, input.uv);
    float depth = sceneDepthTexture.Sample(sceneSampler, input.uv);
    
    float fogFactor = 0.0;
    
    if (fog.enabled)
    {
        float fogRange = max(fog.fogEnd - fog.fogStart, 0.0001);
        fogFactor = saturate((depth - fog.fogStart) / fogRange);
        fogFactor *= saturate(fog.density);
    }

    return lerp(sceneColor, fog.fogColor, fogFactor);
}
