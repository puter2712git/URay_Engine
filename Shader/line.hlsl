struct FrameConstants
{
    float4x4 view;
    float4x4 proj;
};

[[vk::binding(0, 0)]] ConstantBuffer<FrameConstants> frame;

struct VSInput
{
    [[vk::location(0)]] float3 inPosition : POSITION;
    [[vk::location(2)]] float4 inColor : COLOR;
};

struct VSOutput
{
    float4 outPosition : SV_Position;
    [[vk::location(0)]] float4 fragColor : TEXCOORD0;
};

struct PSOutput
{
    [[vk::location(0)]] float4 outColor : SV_Target;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    output.outPosition = mul(frame.proj, mul(frame.view, float4(input.inPosition, 1.0)));
    output.fragColor = input.inColor;

    return output;
}

PSOutput PSMain(VSOutput input)
{
    PSOutput output;

    output.outColor = input.fragColor;

    return output;
}
