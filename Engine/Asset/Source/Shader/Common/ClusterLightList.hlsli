#ifndef __CLUSTER_LIGHT_LIST__
#define __CLUSTER_LIGHT_LIST__

static const uint MaxLightsPerCluster = 64u;

static const uint SpotLightTypeBit = 0x80000000u;
static const uint LightIndexMask = 0x7fffffffu;

struct ClusterLightListEntry
{
    uint lightCount;
    uint overflowCount;
};

uint GetClusterLightListOffset(uint clusterIndex)
{
    return clusterIndex * MaxLightsPerCluster;
}

uint PackPointLightIndex(uint pointLightIndex)
{
    return pointLightIndex;
}

uint PackSpotLightIndex(uint spotLightIndex)
{
    return spotLightIndex | SpotLightTypeBit;
}

bool IsSpotLight(uint packedLightIndex)
{
    return (packedLightIndex & SpotLightTypeBit) != 0u;
}

uint GetLightIndex(uint packedLightIndex)
{
    return packedLightIndex & LightIndexMask;
}

#endif
