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
public:
    static MeshAsset* Import(const std::string& filePath);

private:
    static void Reset();

private:
    struct Face
    {
        std::vector<VertexPNT> vertices;
    };

    static std::vector<Vector3> positions;
    static std::vector<Vector2> uvs;
    static std::vector<Vector3> normals;
    static std::vector<Face> faces;
};

} // namespace URay
