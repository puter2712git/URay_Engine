#pragma once
#include <Core/Type/TypeAlias.h>

#include <Core/Math/Matrix.h>

#include <array>
#include <limits>
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
    Array<SkinInfluence, 4> influences = {};
    uint32 count = 0;
};

struct SkinClusterInfo
{
    uint32 boneIndex = InvalidBoneIndex;
    Matrix inverseBindMatrix = Matrix::Identity;
};

struct BoneInfo
{
    String name;
    int32 parentIndex = -1;

    Matrix worldBindTransform = Matrix::Identity;
};

struct MeshInfo
{
    String name;
    Vector<SkinVertex> skinVertices;
    Vector<SkinClusterInfo> clusters;
};

struct FBXInfo
{
    Vector<BoneInfo> bones;
    Vector<MeshInfo> meshes;
};

} // namespace URay::FBX
