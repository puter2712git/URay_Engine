#include "Common/ClusterGrid.hlsli"
#include "Common/ClusterLightList.hlsli"


[[vk::binding(10, 0)]]
RWStructuredBuffer<ClusterLightListEntry> clusterLightList;

[[vk::binding(11, 0)]]
RWStructuredBuffer<uint> clusterLightIndexList;

[numthreads(1, 1, 1)]
void CSMain(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    const uint clusterIndex = GetClusterIndex(dispatchThreadId);
    
    clusterLightList[clusterIndex].lightCount = 0;
    clusterLightList[clusterIndex].overflowCount = 0;
}
