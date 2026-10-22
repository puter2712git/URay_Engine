#include "Common/FrameConstants.hlsli"

[[vk::binding(0, 2)]]
Texture2D<float4> selectionMaskTexture;

[[vk::binding(1, 2)]]
SamplerState selectionMaskSampler;

struct SelectionOutlineConstants
{
    float4 color;
    float size;
    float threshold;
    float2 padding;
};
[[vk::binding(2, 2)]]
ConstantBuffer<SelectionOutlineConstants> outline;

struct VSOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOut VSMain(uint vertexId : SV_VertexID)
{
    float2 position[3] =
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
    output.position = float4(position[vertexId], 0.0, 1.0);
    output.uv = uvs[vertexId];
    return output;
}

float SampleMask(float2 uv)
{
    return selectionMaskTexture.SampleLevel(selectionMaskSampler, uv, 0.0).r;
}

float4 PSMain(VSOut input) : SV_TARGET
{
    const float center = SampleMask(input.uv);
    const float2 texel = outline.size / frame.renderTargetSize;
    
    float neighbor = 0.0;
    
    neighbor = max(neighbor, SampleMask(input.uv + texel * float2(-1, -1)));
    neighbor = max(neighbor, SampleMask(input.uv + texel * float2(0, -1)));
    neighbor = max(neighbor, SampleMask(input.uv + texel * float2(1, -1)));
    neighbor = max(neighbor, SampleMask(input.uv + texel * float2(-1, 0)));
    neighbor = max(neighbor, SampleMask(input.uv + texel * float2(1, 0)));
    neighbor = max(neighbor, SampleMask(input.uv + texel * float2(-1, 1)));
    neighbor = max(neighbor, SampleMask(input.uv + texel * float2(0, 1)));
    neighbor = max(neighbor, SampleMask(input.uv + texel * float2(1, 1)));
    
    if (center < outline.threshold && neighbor >= outline.threshold)
    {
        return outline.color;
    }
    
    return float4(0.0, 0.0, 0.0, 0.0);
}
