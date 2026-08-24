#ifndef __VERTEX_TYPES__
#define __VERTEX_TYPES__

struct VertexPCIn
{
    [[vk::location(0)]] float3 inPosition : POSITION;
    [[vk::location(2)]] float4 inColor : COLOR;
};

struct VertexPCOut
{
    float4 outPosition : SV_Position;
    [[vk::location(1)]] float4 fragColor : TEXCOORD1;
};

struct FragOut
{
    [[vk::location(0)]] float4 outColor : SV_Target;
};

struct VertexPTCIn
{
    [[vk::location(0)]] float3 inPosition : POSITION;
    [[vk::location(1)]] float2 inUV : TEXCOORD0;
    [[vk::location(2)]] float4 inColor : COLOR;
};

struct VertexPTCOut
{
    float4 outPosition : SV_Position;
    [[vk::location(0)]] float2 outUV : TEXCOORD0;
    [[vk::location(1)]] float4 fragColor : TEXCOORD1;
};

struct VertexPNTIn
{
    [[vk::location(0)]] float3 inPosition : POSITION;
    [[vk::location(1)]] float3 inNormal : NORMAL;
    [[vk::location(2)]] float2 inUV : TEXCOORD0;
};

struct VertexPNTOut
{
    float4 outPosition : SV_Position;
    
    [[vk::location(0)]] float2 outUV : TEXCOORD0;
    [[vk::location(1)]] float3 outNormal : TEXCOORD1;
};

#endif
