#include "ObjImporter.h"

#include "Engine/Engine.h"
#include "Engine/Material/Material.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Texture/Texture.h"
#include "Engine/Texture/TextureManager.h"

#include "Core/File/VirtualFilesystem.h"

#include <iostream>
#include <sstream>
#include <unordered_map>

namespace URay
{

using RHI::VertexPNT;

ObjImporter::ObjImporter(VirtualFilesystem& filesystem,
                         MeshManager& meshManager,
                         TextureManager& textureManager,
                         MaterialManager& materialManager,
                         RHI::Shader* meshShader)
    : filesystem(filesystem), meshManager(meshManager),
      textureManager(textureManager), materialManager(materialManager), meshShader(meshShader)
{
}

Mesh* ObjImporter::Import(const VirtualPath& filePath)
{
    if (!filesystem.Exists(filePath))
    {
        return nullptr;
    }

    Reset();

    ParseObj(filePath);

    VirtualPath mtlPath = filePath.GetDirectory().Join(mtllib);
    ParseMtl(mtlPath);

    std::vector<VertexPNT> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<ObjIndex, uint32_t, ObjIndexHash> vertexMap;

    auto AddVertex = [&](const ObjIndex& objIndex) -> uint32_t
    {
        auto it = vertexMap.find(objIndex);
        if (it != vertexMap.end())
            return it->second;

        VertexPNT vertex = {};
        vertex.position = positions[objIndex.posIndex];

        if (objIndex.uvIndex >= 0)
        {
            vertex.uv = uvs[objIndex.uvIndex];
            vertex.uv.y = 1.0f - vertex.uv.y;
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

    MaterialImportResult materialImportResult = CreateMaterials();
    std::vector<Material*>& materials = materialImportResult.materials;
    std::unordered_map<std::string, uint32_t>& materialSlots = materialImportResult.slots;

    if (materials.empty())
    {
        Material* defaultMaterial = materialManager.GetOrCreate("Mesh", meshShader);
        materials.push_back(defaultMaterial);
    }

    std::vector<MeshSection> sections;

    uint32_t activeMaterialSlot = UINT32_MAX;
    MeshSection activeSection = {};

    for (const Face& face : faces)
    {
        uint32_t materialSlot = 0;

        auto it = materialSlots.find(face.mtlName);
        if (it != materialSlots.end())
        {
            materialSlot = it->second;
        }

        if (activeMaterialSlot != materialSlot)
        {
            if (activeSection.indexCount > 0)
            {
                sections.push_back(activeSection);
            }

            activeMaterialSlot = materialSlot;
            activeSection = {
                .indexOffset = static_cast<uint32_t>(indices.size()),
                .indexCount = 0,
                .materialIndex = materialSlot
            };
        }

        for (size_t i = 1; i + 1 < face.objIndice.size(); ++i)
        {
            indices.push_back(AddVertex(face.objIndice[0]));
            indices.push_back(AddVertex(face.objIndice[i]));
            indices.push_back(AddVertex(face.objIndice[i + 1]));
            activeSection.indexCount += 3;
        }
    }

    if (activeSection.indexCount > 0)
    {
        sections.push_back(activeSection);
    }

    Mesh* mesh = meshManager.CreateMesh(filePath.ToString(), vertices, indices);
    mesh->SetSections(sections);
    mesh->SetDefaultMaterials(materials);

    return mesh;
}

void ObjImporter::Reset()
{
    positions.clear();
    uvs.clear();
    normals.clear();
    faces.clear();

    mtllib.clear();
    mtlInfos.clear();
}

void ObjImporter::ParseObj(const VirtualPath& objPath)
{
    std::string fileText = filesystem.ReadText(objPath);
    std::istringstream fileStream(fileText);

    std::string currMtlName;

    std::string line;
    while (std::getline(fileStream, line))
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
        else if (type == "usemtl")
        {
            ss >> currMtlName;
        }
        else if (type == "f")
        {
            Face face = ParseFace(line);
            face.mtlName = currMtlName;
            faces.push_back(std::move(face));
        }
        else if (type == "mtllib")
        {
            ss >> mtllib;
        }
    }
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

void ObjImporter::ParseMtl(const VirtualPath& mtlPath)
{
    std::string fileText = filesystem.ReadText(mtlPath);
    std::istringstream fileStream(fileText);

    bool isFirstMtl = true;
    MtlInfo mtlInfo = {};

    std::string line;
    while (std::getline(fileStream, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "newmtl")
        {
            if (!isFirstMtl)
            {
                mtlInfos.push_back(mtlInfo);
            }

            mtlInfo = {};
            ss >> mtlInfo.mtlName;
            isFirstMtl = false;
        }
        else if (type == "Ns")
        {
            ss >> mtlInfo.specularExponent;
        }
        else if (type == "Ka")
        {
            ss >> mtlInfo.ambient.x >> mtlInfo.ambient.y >> mtlInfo.ambient.z;
        }
        else if (type == "Ks")
        {
            ss >> mtlInfo.specular.x >> mtlInfo.specular.y >> mtlInfo.specular.z;
        }
        else if (type == "Ke")
        {
            ss >> mtlInfo.emissive.x >> mtlInfo.emissive.y >> mtlInfo.emissive.z;
        }
        else if (type == "Ni")
        {
            ss >> mtlInfo.refractiveIndex;
        }
        else if (type == "map_Kd")
        {
            std::string path;
            ss >> path;
            mtlInfo.diffuseTexturePath = mtlPath.GetDirectory().Join(path);
        }
        else if (type == "map_d")
        {
            std::string path;
            ss >> path;
            mtlInfo.alphaTexturePath = mtlPath.GetDirectory().Join(path);
        }
    }

    if (!isFirstMtl)
    {
        mtlInfos.push_back(mtlInfo);
    }
}

ObjImporter::MaterialImportResult ObjImporter::CreateMaterials()
{
    MaterialImportResult result = {};

    for (const MtlInfo& info : mtlInfos)
    {
        Material* material = materialManager.GetOrCreate(info.mtlName, meshShader);

        if (!info.diffuseTexturePath.ToString().empty())
        {
            const std::string textureKey = info.diffuseTexturePath.ToString();

            Texture* texture = textureManager.GetTexture(textureKey);
            if (!texture)
            {
                texture = textureManager.LoadTexture(textureKey, info.diffuseTexturePath);
            }

            if (texture)
            {
                material->SetTexture(texture);
            }
        }

        uint32_t slot = static_cast<uint32_t>(result.materials.size());

        result.materials.push_back(material);
        result.slots.insert({ info.mtlName, slot });
    }

    return result;
}

} // namespace URay
