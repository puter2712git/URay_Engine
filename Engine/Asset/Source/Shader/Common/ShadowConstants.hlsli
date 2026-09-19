#ifndef __SHADOW_CONSTANTS__
#define __SHADOW_CONSTANTS__

struct ShadowConstants
{
    float4x4 lightViewProj;
    float bias;
    float3 padding;
};
[[vk::binding(3, 0)]] ConstantBuffer<ShadowConstants> directionalShadow;

[[vk::binding(4, 0)]]
Texture2D<float> directionalShadowDepth;

[[vk::binding(5, 0)]]
SamplerState directionalShadowSampler;

[[vk::binding(6, 0)]]
Texture2D<float> shadowAtlas;

struct SpotLightShadowConstants
{
    float4x4 lightViewProj;
    float4 atlasUVScaleBias;
    float bias;
    float3 padding2;
};

[[vk::binding(7, 0)]]
StructuredBuffer<SpotLightShadowConstants> spotLightShadows;

#endif
