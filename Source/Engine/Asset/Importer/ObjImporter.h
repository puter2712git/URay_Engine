#pragma once

#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"

#include "Render/Vertex.h"

#include <string>
#include <vector>

namespace URay
{

class MeshAsset;

class ObjImporter
{
private:
    struct ObjIndex
    {
        int posIndex = -1;
        int uvIndex = -1;
        int normalIndex = -1;

        bool operator==(const ObjIndex&) const = default;
    };

    struct ObjIndexHash
    {
        size_t operator()(const ObjIndex& k) const
        {
            size_t h1 = std::hash<int>{}(k.posIndex);
            size_t h2 = std::hash<int>{}(k.uvIndex);
            size_t h3 = std::hash<int>{}(k.normalIndex);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    struct Face
    {
        std::vector<ObjIndex> objIndice;
    };

public:
    static MeshAsset* Import(const std::string& filePath);

private:
    static void Reset();

    static Face ParseFace(const std::string& line);

    static ObjIndex ParseObjIndex(const std::string& token);

private:
    static std::vector<Vector3> positions;
    static std::vector<Vector2> uvs;
    static std::vector<Vector3> normals;
    static std::vector<Face> faces;
};

} // namespace URay
