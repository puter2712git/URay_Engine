#include "ObjImporter.h"

#include "Engine/Engine.h"
#include "Engine/Mesh/MeshManager.h"

#include "Core/File/FileIO.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace URay
{

std::vector<Vector3> ObjImporter::positions;
std::vector<Vector2> ObjImporter::uvs;
std::vector<Vector3> ObjImporter::normals;
std::vector<ObjImporter::Face> ObjImporter::faces;

MeshAsset* ObjImporter::Import(const std::string& filePath)
{
    if (!FileIO::Exists(filePath))
        return nullptr;

    Reset();

    std::fstream file(filePath);
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v")
        {
            Vector3 p;
            ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (type == "vt")
        {
            Vector2 uv;
            ss >> uv.x >> uv.y;
            uvs.push_back(uv);
        }
        else if (type == "vn")
        {
            Vector3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (type == "f")
        {
            Face face = ParseFace(line);
            faces.push_back(face);
        }
    }

    std::vector<VertexPNT> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<ObjIndex, uint32_t, ObjIndexHash> vertexMap;

    auto AddVertex = [&](const ObjIndex& objIndex) -> uint32_t {
        auto it = vertexMap.find(objIndex);
        if (it != vertexMap.end())
            return it->second;

        VertexPNT vertex = {};
        vertex.position = positions[objIndex.posIndex];

        if (objIndex.uvIndex >= 0)
        {
            vertex.uv = uvs[objIndex.uvIndex];
        }

        if (objIndex.normalIndex >= 0)
        {
            vertex.normal = normals[objIndex.normalIndex];
        }

        uint32_t newIndex = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
        vertexMap.insert({ objIndex, newIndex });

        return newIndex;
    };

    for (const Face& face : faces)
    {
        if (face.objIndice.size() < 3)
            continue;

        for (size_t i = 1; i + 1 < face.objIndice.size(); ++i)
        {
            indices.push_back(AddVertex(face.objIndice[0]));
            indices.push_back(AddVertex(face.objIndice[i]));
            indices.push_back(AddVertex(face.objIndice[i + 1]));
        }
    }

    MeshManager* meshManager = gEngine->GetMeshManager();
    MeshAsset* newMeshAsset = meshManager->CreateMesh(filePath, vertices, indices);
    return newMeshAsset;
}

void ObjImporter::Reset()
{
    positions.clear();
    uvs.clear();
    normals.clear();
    faces.clear();
}

ObjImporter::Face ObjImporter::ParseFace(const std::string& line)
{
    Face face;
    
    std::stringstream ss(line);
    std::string prefix, token;

    ss >> prefix;
    while (ss >> token)
    {
        ObjIndex objIndex = ParseObjIndex(token);
        face.objIndice.push_back(objIndex);
    }

    return face;
}

ObjImporter::ObjIndex ObjImporter::ParseObjIndex(const std::string& token)
{
    ObjIndex objIndex = {};

    std::stringstream ss(token);
    std::string part;

    if (std::getline(ss, part, '/') && !part.empty())
    {
        objIndex.posIndex = std::stoi(part) - 1;
    }

    if (std::getline(ss, part, '/') && !part.empty())
    {
        objIndex.uvIndex = std::stoi(part) - 1;
    }

    if (std::getline(ss, part, '/') && !part.empty())
    {
        objIndex.normalIndex = std::stoi(part) - 1;
    }

    return objIndex;
}

} // namespace URay
