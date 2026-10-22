struct VertexUIIn
{
    [[vk::location(0)]] float2 inPosition : POSITION;
    [[vk::location(1)]] float2 inUV : TEXCOORD0;
    [[vk::location(2)]] float4 inColor : COLOR;
};

struct VertexUIOut
{
    float4 outPosition : SV_Position;
    [[vk::location(0)]] float4 outColor : COLOR;
    [[vk::location(1)]] float2 outUV : TEXCOORD0;
};

struct FragOut
{
    [[vk::location(0)]] float4 outColor : SV_Target;
};

struct UIConstants
{
    float2 viewportSize;
    float2 padding;
};
[[vk::push_constant]]
UIConstants ui;

[[vk::binding(0, 1)]] Texture2D<float4> textureImage;
[[vk::binding(1, 1)]] SamplerState textureSampler;

VertexUIOut VSMain(VertexUIIn input)
{
    VertexUIOut output;
    
    float2 ndc;
    ndc.x = (input.inPosition.x / ui.viewportSize.x) * 2.0f - 1.0f;
    ndc.y = 1.0f - (input.inPosition.y / ui.viewportSize.y) * 2.0f;
    
    output.outPosition = float4(ndc, 0.0f, 1.0f);
    output.outColor = input.inColor;
    output.outUV = input.inUV;
    
    return output;
}

FragOut PSMain(VertexUIOut input)
{
    FragOut output;
    output.outColor = textureImage.Sample(textureSampler, input.outUV) * input.outColor;
    return output;
}
