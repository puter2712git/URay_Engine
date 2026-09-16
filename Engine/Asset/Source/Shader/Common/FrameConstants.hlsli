#ifndef __FRAME_CONSTANTS__
#define __FRAME_CONSTANTS__

#include "Common/Light.hlsli"

struct FrameConstants
{
    float4x4 view;
    float4x4 invView;
    float4x4 proj;
    float4x4 invProj;
    float4x4 viewProj;
    float4x4 invViewProj;
    float nearPlane;
    float farPlane;
    float2 renderTargetSize;
    
    AmbientLightConstants ambientLight;
    DirectionalLightConstants directionalLight;
};
[[vk::binding(0, 0)]] ConstantBuffer<FrameConstants> frame;

#endif
