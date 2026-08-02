struct ObjectConstants
{
    float4x4 world;
    float4 colorTint;
    uint objectId;
};
[[vk::push_constant]] ObjectConstants obj;

struct FrameConstants
{
    float4x4 view;
    float4x4 proj;
};
[[vk::binding(0, 0)]] ConstantBuffer<FrameConstants> frame;

[[vk::binding(0, 1)]] Texture2D<float4> textureImage;
[[vk::binding(1, 1)]] SamplerState textureSampler;

struct VSInput
{
    [[vk::location(0)]] float3 inPosition : POSITION;
    [[vk::location(1)]] float2 inUV : TEXCOORD0;
    [[vk::location(2)]] float4 inColor : COLOR;
};

struct VSOutput
{
    float4 outPosition : SV_Position;
    [[vk::location(0)]] float2 outUV : TEXCOORD0;
    [[vk::location(1)]] float4 fragColor : TEXCOORD1;
};

struct PSOutput
{
    [[vk::location(0)]] float4 outColor : SV_Target;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    output.outPosition = mul(frame.proj, mul(frame.view, mul(obj.world, float4(input.inPosition, 1.0))));
    output.outUV = input.inUV;
    output.fragColor = input.inColor;

    return output;
}

PSOutput PSMain(VSOutput input)
{
    PSOutput output;

    output.outColor = textureImage.Sample(textureSampler, input.outUV) * input.fragColor * obj.colorTint;

    return output;
}
