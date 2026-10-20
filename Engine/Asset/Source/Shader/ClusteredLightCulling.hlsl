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
    
    for (uint i = 0; i < frame.pointLightCount; ++i)
    {
        const float3 pointLightViewPosition = mul(frame.view, float4(pointLights[i].position, 1.0)).xyz;

        if (!IntersectsSphereAABB(pointLightViewPosition, pointLights[i].radius, aabbMin, aabbMax))
            continue;
        
        if (lightCount < MaxLightsPerCluster)
        {
            const uint offset = GetClusterLightListOffset(clusterIndex);
            
            clusterLightIndexList[offset + lightCount] = PackPointLightIndex(i);
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
