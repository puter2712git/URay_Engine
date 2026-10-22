#include "Common/ClusterGrid.hlsli"
#include "Common/ClusterLightList.hlsli"
#include "Common/ClusterCulling.hlsli"
#include "Common/FrameConstants.hlsli"

[[vk::binding(10, 0)]]
RWStructuredBuffer<ClusterLightListEntry> clusterLightList;

[[vk::binding(11, 0)]]
RWStructuredBuffer<uint> clusterLightIndexList;

[numthreads(1, 1, 1)]
void CSMain(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    const uint clusterIndex = GetClusterIndex(dispatchThreadId);
    
    float3 aabbMin;
    float3 aabbMax;
    GetClusterViewAABB(dispatchThreadId, aabbMin, aabbMax);
    
    uint lightCount = 0;
    uint overflowCount = 0;
    
    const uint offset = GetClusterLightListOffset(clusterIndex);
    
    // PointLight Loop
    for (uint i = 0; i < frame.pointLightCount; ++i)
    {
        const float3 pointLightViewPosition = mul(frame.view, float4(pointLights[i].position, 1.0)).xyz;

        if (!IntersectsSphereAABB(pointLightViewPosition, pointLights[i].radius, aabbMin, aabbMax))
            continue;
        
        if (lightCount < MaxLightsPerCluster)
        {
            clusterLightIndexList[offset + lightCount] = PackPointLightIndex(i);
            ++lightCount;
        }
        else
        {
            ++overflowCount;
        }
    }
    
    // SpotLight Loop
    for (uint i = 0; i < frame.spotLightCount; ++i)
    {
        const SpotLight light = spotLights[i];
        
        const float range = light.range;
        const float baseRadius = range * tan(light.outerConeAngle);
        
        // Calculate the bounding sphere center in world space
        const float halfRange = range * 0.5;
        const float3 direction = normalize(light.direction);
        const float3 sphereWorldCenter = light.position + direction * halfRange;
        const float sphereRadius = sqrt(halfRange * halfRange + baseRadius * baseRadius);
        
        const float3 sphereViewCenter = mul(frame.view, float4(sphereWorldCenter, 1.0)).xyz;

        if (!IntersectsSphereAABB(sphereViewCenter, sphereRadius, aabbMin, aabbMax))
            continue;
        
        if (lightCount < MaxLightsPerCluster)
        {
            clusterLightIndexList[offset + lightCount] = PackSpotLightIndex(i);
            ++lightCount;
        }
        else
        {
            ++overflowCount;
        }
    }
    
    clusterLightList[clusterIndex].lightCount = lightCount;
    clusterLightList[clusterIndex].overflowCount = overflowCount;
}
