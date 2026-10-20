#include "Common/ClusterGrid.hlsli"

struct ClusterLightListEntry
{
    uint lightCount;
    uint overflowCount;
};

[[vk::binding(10, 0)]]
RWStructuredBuffer<ClusterLightListEntry> clusterLightList;

[numthreads(1, 1, 1)]
void CSMain(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    const uint clusterIndex = GetClusterIndex(dispatchThreadId);
    
    clusterLightList[clusterIndex].lightCount = 0;
    clusterLightList[clusterIndex].overflowCount = 0;
}
