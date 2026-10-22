#ifndef __CLUSTER_GRID__
#define __CLUSTER_GRID__

static const uint ClusterGridCountX = 16u;
static const uint ClusterGridCountY = 9u;
static const uint ClusterGridCountZ = 24u;

static const uint ClusterCount = ClusterGridCountX * ClusterGridCountY * ClusterGridCountZ;

uint GetClusterIndex(uint3 clusterCoord)
{
    return
        clusterCoord.x +
        clusterCoord.y * ClusterGridCountX +
        clusterCoord.z * ClusterGridCountX * ClusterGridCountY;
}

#endif
