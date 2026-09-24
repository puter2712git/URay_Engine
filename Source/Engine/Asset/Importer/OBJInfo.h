#pragma once

#include "Core/Math/Color3.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Type/Types.h"

#include <string>
#include <vector>

namespace URay::OBJ
{

struct OBJIndex
{
    int32 positionIndex = -1;
    int32 normalIndex = -1;
    int32 uvIndex = -1;
};

struct Face
{
    std::vector<OBJIndex> indices;
};

struct OBJInfo
{
    std::string mtllib;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> uvs;

    std::vector<Face> faces;
};

struct MTLData
{
    std::string mtlName;
    float shininess = 0.0f;
    Color3 ambient = Color3::White;
    Color3 specular = Color3::White;
    Color3 emissive = Color3::Black;
    float refractiveIndex = 0.0f;
    float dissolve = 1.0f;
    int32 illuminationModel = 2;

    std::string diffuseTexturePath;
};

struct MTLInfo
{
    std::vector<MTLData> datas;
};

} // namespace URay::OBJ
