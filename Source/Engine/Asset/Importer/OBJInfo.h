#pragma once
#include <Core/Type/TypeAlias.h>

#include <Core/Math/Color3.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>

#include <unordered_map>
#include <vector>

namespace URay::OBJ
{

struct OBJIndex
{
    int32 positionIndex = -1;
    int32 normalIndex = -1;
    int32 uvIndex = -1;

    bool operator==(const OBJIndex&) const = default;
};

struct OBJIndexHash
{
    size_t operator()(const OBJIndex& index) const
    {
        size_t hash = 0;

        auto combine = [&hash](auto value)
        {
            const size_t valueHash = std::hash<decltype(value)>{}(value);
            hash ^= valueHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        };

        combine(index.positionIndex);
        combine(index.normalIndex);
        combine(index.uvIndex);

        return hash;
    }
};

struct Face
{
    Vector<OBJIndex> indices;
};

struct OBJInfo
{
    String mtllib;

    Vector<Vector3> positions;
    Vector<Vector3> normals;
    Vector<Vector2> uvs;

    std::unordered_map<String, Vector<Face>> mtlTofaces;
};

struct MTLData
{
    String mtlName;
    float shininess = 0.0f;
    Color3 ambient = Color3::White;
    Color3 specular = Color3::White;
    Color3 emissive = Color3::Black;
    float refractiveIndex = 0.0f;
    float dissolve = 1.0f;
    int32 illuminationModel = 2;

    String diffuseTexturePath;
};

struct MTLInfo
{
    Vector<MTLData> datas;
};

} // namespace URay::OBJ
