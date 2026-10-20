#ifndef __CLUSTER_CULLING__
#define __CLUSTER_CULLING__

#include "Common/ClusterGrid.hlsli"
#include "Common/FrameConstants.hlsli"

float GetClusterSliceDepth(uint slice)
{
    const float t = float(slice) / float(ClusterGridCountZ);
    
    return frame.nearPlane * pow(frame.farPlane / frame.nearPlane, t);
}

float GetDeviceDepthFromLinearDepth(float linearDepth)
{
    const float n = frame.nearPlane;
    const float f = frame.farPlane;
    
    return f / (f - n) -
        (n * f) / ((f - n) * linearDepth);
}

float3 ReconstructViewPosition(float2 ndcXY, float deviceDepth)
{
    float4 viewPosition = mul(frame.invProj, float4(ndcXY, deviceDepth, 1.0));
    return viewPosition.xyz / viewPosition.w;
}

void GetClusterViewAABB(uint3 clusterCoord, out float3 aabbMin, out float3 aabbMax)
{
    const float2 ndcMin = float2(
        -1.0 + 2.0 * float(clusterCoord.x) / float(ClusterGridCountX),
        1.0 - 2.0 * float(clusterCoord.y + 1u) / float(ClusterGridCountY));
    
    const float2 ndcMax = float2(
        -1.0 + 2.0 * float(clusterCoord.x + 1u) / float(ClusterGridCountX),
        1.0 - 2.0 * float(clusterCoord.y) / float(ClusterGridCountY));
    
    const float nearDepth = GetClusterSliceDepth(clusterCoord.z);
    const float farDepth = GetClusterSliceDepth(clusterCoord.z + 1u);
    
    const float nearDeviceDepth = GetDeviceDepthFromLinearDepth(nearDepth);
    const float farDeviceDepth = GetDeviceDepthFromLinearDepth(farDepth);
    
    const float3 p0 = ReconstructViewPosition(float2(ndcMin.x, ndcMin.y), nearDeviceDepth);
    const float3 p1 = ReconstructViewPosition(float2(ndcMax.x, ndcMin.y), nearDeviceDepth);
    const float3 p2 = ReconstructViewPosition(float2(ndcMin.x, ndcMax.y), nearDeviceDepth);
    const float3 p3 = ReconstructViewPosition(float2(ndcMax.x, ndcMax.y), nearDeviceDepth);

    const float3 p4 = ReconstructViewPosition(float2(ndcMin.x, ndcMin.y), farDeviceDepth);
    const float3 p5 = ReconstructViewPosition(float2(ndcMax.x, ndcMin.y), farDeviceDepth);
    const float3 p6 = ReconstructViewPosition(float2(ndcMin.x, ndcMax.y), farDeviceDepth);
    const float3 p7 = ReconstructViewPosition(float2(ndcMax.x, ndcMax.y), farDeviceDepth);
    
    aabbMin = min(min(min(p0, p1), min(p2, p3)), min(min(p4, p5), min(p6, p7)));
    aabbMax = max(max(max(p0, p1), max(p2, p3)), max(max(p4, p5), max(p6, p7)));
}

bool IntersectsSphereAABB(
    float3 sphereCenter,
    float sphereRadius,
    float3 aabbMin,
    float3 aabbMax)
{
    const float3 closestPoint = clamp(sphereCenter, aabbMin, aabbMax);
    const float3 delta = sphereCenter - closestPoint;
    
    return dot(delta, delta) <= sphereRadius * sphereRadius;
}

#endif
