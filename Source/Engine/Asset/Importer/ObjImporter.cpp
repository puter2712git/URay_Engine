#include "ObjImporter.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/LogSystem.h"
#include "Core/Type/Types.h"

#include <iostream>
#include <sstream>
#include <unordered_map>

namespace URay
{

OBJImporter::OBJImporter() = default;

OBJImporter::~OBJImporter() = default;

void OBJImporter::Import(const VirtualPath& sourcePath)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    AssetMetadata metadata = {};

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    const VirtualPath metadataPath = VirtualPath(importPath.ToString() + ".meta");

    if (fileSystem.Exists(metadataPath))
    {
        const std::string fileText = fileSystem.ReadText(metadataPath);
        const YAML::Node node = YAML::Load(fileText);
        metadata.Deserialize(node);
    }
    else
    {
        metadata = CreateMetadata(sourcePath);
        const YAML::Node node = metadata.Serialize();
        fileSystem.WriteText(metadataPath, YAML::Dump(node));
    }

    OBJ::OBJInfo objInfo = ParseOBJ(sourcePath);

    const VirtualPath objDirectory = sourcePath.GetDirectory();
    const VirtualPath mtlPath = objDirectory.Join(objInfo.mtllib);

    OBJ::MTLInfo mtlInfo = ParseMTL(mtlPath);

    URAY_LOG(
        "[OBJImporter] OBJ Info:\n"
        "\tmtllib: %s\n"
        "\tpositions size: %zu\n"
        "\tnormals size: %zu\n"
        "\tuvs size: %zu\n"
        "\tfaces size: %zu\n",
        objInfo.mtllib.c_str(),
        objInfo.positions.size(),
        objInfo.normals.size(),
        objInfo.uvs.size(),
        objInfo.faces.size());

    URAY_LOG("[OBJImporter] MTL Info:\n");
    for (const OBJ::MTLData& data : mtlInfo.datas)
    {
        URAY_LOG(
            "Name: %s"
            "\tshininess: %f"
            "\tambient: %f"
            "\tspecular: %f"
            "\temissive: %f"
            "\trefractiveIndex: %d"
            "\tdissolve: %f"
            "\tilluminationModel: %d"
            "\tdiffuseTexturePath: %s",
            data.mtlName.c_str(),
            data.shininess,
            data.ambient,
            data.specular,
            data.emissive,
            data.refractiveIndex,
            data.dissolve,
            data.illuminationModel,
            data.diffuseTexturePath.c_str());
    }

    Assemble(objInfo, mtlInfo, metadata);
}

AssetMetadata OBJImporter::CreateMetadata(const VirtualPath& sourcePath) const
{
    AssetMetadata ret = {};

    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    ret.handle = AssetHandle::Generate();
    ret.type = AssetType::Material;
    ret.sourcePath = sourcePath;

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    ret.metadataPath = VirtualPath(importPath.ToString() + ".meta");
    ret.assetPath = VirtualPath(importPath.ToString() + ".asset");

    return ret;
}

bool OBJImporter::CanImport(const std::string& extension) const
{
    return extension == ".obj";
}

void OBJImporter::Assemble(const OBJ::OBJInfo& objInfo, const OBJ::MTLInfo& mtlInfo, const AssetMetadata& metadata) const
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;

    std::unordered_map<OBJ::OBJIndex, uint32, OBJ::OBJIndexHash> vertexMap;

    for (const OBJ::Face& face : objInfo.faces)
    {
        for (size_t i = 1; i + 1 < face.indices.size(); ++i)
        {
            indices.push_back(
                GetOrCreateVertex(objInfo, face.indices[0], vertices, vertexMap));

            indices.push_back(
                GetOrCreateVertex(objInfo, face.indices[i], vertices, vertexMap));

            indices.push_back(
                GetOrCreateVertex(objInfo, face.indices[i + 1], vertices, vertexMap));
        }
    }

    sections.push_back(MeshSection{
        .indexOffset = 0,
        .indexCount = static_cast<uint32>(indices.size()),
        .materialIndex = 0 });

    auto mesh = std::make_unique<Mesh>();
    mesh->SetHandle(metadata.handle);
    mesh->SetName(metadata.sourcePath.GetStem());
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    mesh->SetSections(sections);
    mesh->SetDefaultMaterials({ EngineAsset::MeshMaterial });

    assetDatabase.Add(std::move(mesh));
}

OBJ::OBJInfo OBJImporter::ParseOBJ(const VirtualPath& objPath) const
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    const std::string fileText = fileSystem.ReadText(objPath);
    if (fileText.empty())
        return {};

    OBJ::OBJInfo objInfo = {};

    std::istringstream stream(fileText);

    std::string line;
    while (std::getline(stream, line))
    {
        std::istringstream lineStream(line);

        std::string type;
        lineStream >> type;

        if (type == "mtllib")
        {
            lineStream >> objInfo.mtllib;
        }
        else if (type == "v")
        {
            Vector3 position;
            lineStream >> position.x >> position.y >> position.z;
            objInfo.positions.push_back(position);
        }
        else if (type == "vn")
        {
            Vector3 normal;
            lineStream >> normal.x >> normal.y >> normal.z;
            objInfo.normals.push_back(normal);
        }
        else if (type == "vt")
        {
            Vector2 uv;
            lineStream >> uv.x >> uv.y;
            objInfo.uvs.push_back(uv);
        }
        else if (type == "f")
        {
            OBJ::Face face;

            std::string vertexString;

            while (lineStream >> vertexString)
            {
                OBJ::OBJIndex objIndex = ParseOBJIndex(vertexString);
                face.indices.push_back(objIndex);
            }

            objInfo.faces.push_back(face);
        }
    }

    return objInfo;
}

OBJ::MTLInfo OBJImporter::ParseMTL(const VirtualPath& mtlPath) const
{
    OBJ::MTLInfo mtlInfo = {};

    return mtlInfo;
}

OBJ::OBJIndex OBJImporter::ParseOBJIndex(const std::string& vertexString) const
{
    OBJ::OBJIndex objIndex = {};

    std::istringstream stream(vertexString);

    std::string value;

    if (std::getline(stream, value, '/'))
    {
        if (!value.empty())
            objIndex.positionIndex = std::stoi(value) - 1;
    }

    if (std::getline(stream, value, '/'))
    {
        if (!value.empty())
            objIndex.uvIndex = std::stoi(value) - 1;
    }

    if (std::getline(stream, value, '/'))
    {
        if (!value.empty())
            objIndex.normalIndex = std::stoi(value) - 1;
    }

    return objIndex;
}

uint32 OBJImporter::GetOrCreateVertex(
    const OBJ::OBJInfo& objInfo,
    const OBJ::OBJIndex& objIndex,
    std::vector<Render::VertexPNT>& verticesRef,
    std::unordered_map<OBJ::OBJIndex, uint32, OBJ::OBJIndexHash>& vertexMap) const
{
    const auto it = vertexMap.find(objIndex);
    if (it != vertexMap.end())
        return it->second;

    Render::VertexPNT vertex = {};
    vertex.position = objInfo.positions[objIndex.positionIndex];

    if (objIndex.normalIndex >= 0)
        vertex.normal = objInfo.normals[objIndex.normalIndex];

    if (objIndex.uvIndex >= 0)
        vertex.uv = objInfo.uvs[objIndex.uvIndex];

    uint32 index = static_cast<uint32>(verticesRef.size());

    verticesRef.push_back(vertex);
    vertexMap.emplace(objIndex, index);

    return index;
}

} // namespace URay
