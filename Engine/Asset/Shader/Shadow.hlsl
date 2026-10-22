#include "Common/FrameConstants.hlsli"

#ifndef URAY_SKINNED_MESH
#define URAY_SKINNED_MESH 0
#endif

struct ShadowMapConstants
{
    float4x4 world;
    float4x4 lightViewProj;
};
[[vk::push_constant]] ShadowMapConstants shadowMap;

#if URAY_SKINNED_MESH
[[vk::binding(0, 2)]]
StructuredBuffer<float4x4> skinMatrices;
#endif

struct ShadowVertexIn
{
    [[vk::location(0)]] float3 inPosition : POSITION;
};

struct ShadowSkinnedVertexIn
{
    [[vk::location(0)]] float3 inPosition : POSITION;
    [[vk::location(3)]] uint4 inBoneIndices : BLENDINDICES;
    [[vk::location(4)]] float4 inBoneWeights : BLENDWEIGHT;
};

struct ShadowVertexOut
{
    float4 outPosition : SV_Position;
};

ShadowVertexOut VSMain(
#if URAY_SKINNED_MESH
    ShadowSkinnedVertexIn input
#else
    ShadowVertexIn input
#endif
)
{
    ShadowVertexOut output;

    float4 localPosition = float4(input.inPosition, 1.0);

#if URAY_SKINNED_MESH
    localPosition = 0.0.xxxx;

    for (uint influenceIndex = 0; influenceIndex < 4; ++influenceIndex)
    {
        const float boneWeight = input.inBoneWeights[influenceIndex];

        if (boneWeight == 0.0)
            continue;

        localPosition += boneWeight * mul(
            skinMatrices[input.inBoneIndices[influenceIndex]],
            float4(input.inPosition, 1.0));
    }
#endif

    const float4 worldPosition = mul(shadowMap.world, localPosition);

    output.outPosition = mul(shadowMap.lightViewProj, worldPosition);
    
    return output;
}

void PSMain(ShadowVertexOut input)
{
}
