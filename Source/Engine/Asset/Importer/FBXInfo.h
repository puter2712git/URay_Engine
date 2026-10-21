#pragma once

#include <Core/Type/Types.h>

#include <array>
#include <limits>
#include <string>
#include <vector>

namespace URay::FBX
{

inline constexpr uint32 InvalidBoneIndex = std::numeric_limits<uint32>::max();

struct SkinInfluence
{
    uint32 boneIndex = 0;
    float weight = 0.0f;
};

struct SkinVertex
{
    std::array<SkinInfluence, 4> influences = {};
    uint32 count = 0;
};

struct BoneInfo
{
    std::string name;
    int32 parentIndex = -1;
};

struct MeshInfo
{
    std::string name;
    std::vector<SkinVertex> skinVertices;
    std::vector<uint32> clusterToBoneIndices;
};

struct FBXInfo
{
    std::vector<BoneInfo> bones;
    std::vector<MeshInfo> meshes;
};

} // namespace URay::FBX
