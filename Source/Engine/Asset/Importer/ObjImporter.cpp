#include "ObjImporter.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>

#include <Core/File/VirtualFilesystem.h>
#include <Core/Log/LogSystem.h>
#include <Core/Type/Types.h>

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

void OBJImporter::Assemble(
    const OBJ::OBJInfo& objInfo,
    const OBJ::MTLInfo& mtlInfo,
    const AssetMetadata& metadata) const
{
    const VirtualPath directory = metadata.sourcePath.GetDirectory();

    std::vector<Material*> materials = CreateMaterials(mtlInfo, directory);
    CreateMesh(objInfo, materials, metadata);
}

std::vector<Material*> OBJImporter::CreateMaterials(
    const OBJ::MTLInfo& mtlInfo,
    const VirtualPath& directory) const
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    std::vector<Material*> materials;

    for (const OBJ::MTLData& mtlData : mtlInfo.datas)
    {
        auto newMaterial = std::make_unique<Material>(EngineAsset::MeshShader);
        newMaterial->SetName(mtlData.mtlName);
        newMaterial->SetHandle(UUID::Generate());

        if (!mtlData.diffuseTexturePath.empty())
        {
            const VirtualPath diffuseTexturePath = directory.Join(mtlData.diffuseTexturePath);
            const VirtualPath diffuseTextureImportPath = assetSystem.GetImportAssetPath(diffuseTexturePath);
            const VirtualPath diffuseTextureMetadataPath = VirtualPath(diffuseTextureImportPath.ToString() + ".meta");

            const std::string metadataText = fileSystem.ReadText(diffuseTextureMetadataPath);
            if (!metadataText.empty())
            {
                AssetMetadata metadata = {};

                const YAML::Node metadataNode = YAML::Load(metadataText);
                metadata.Deserialize(metadataNode);

                newMaterial->AddParameter(
                    "diffuseColorTexture",
                    MaterialParameterType::Texture2D,
                    metadata.handle);
            }
        }

        materials.push_back(newMaterial.get());

        assetDatabase.Add(std::move(newMaterial));
    }

    return materials;
}

void OBJImporter::CreateMesh(
    const OBJ::OBJInfo& objInfo,
    const std::vector<Material*>& materials,
    const AssetMetadata& metadata) const
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;

    std::unordered_map<OBJ::OBJIndex, uint32, OBJ::OBJIndexHash> vertexMap;

    for (const auto& [mtlName, faces] : objInfo.mtlTofaces)
    {
        MeshSection section = {};
        section.indexOffset = static_cast<uint32>(indices.size());

        for (const OBJ::Face& face : faces)
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

        section.indexCount = static_cast<uint32>(indices.size()) - section.indexOffset;

        for (uint32 materialIndex = 0; materialIndex < static_cast<uint32>(materials.size()); ++materialIndex)
        {
            if (materials[materialIndex]->GetName() == mtlName)
            {
                section.materialIndex = materialIndex;
                break;
            }
        }

        sections.push_back(section);
    }

    auto mesh = std::make_unique<Mesh>();
    mesh->SetHandle(metadata.handle);
    mesh->SetName(metadata.sourcePath.GetStem());
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    mesh->SetSections(sections);

    if (materials.empty())
    {
        mesh->SetDefaultMaterials({ EngineAsset::MeshMaterial });
    }
    else
    {
        std::vector<AssetHandle> materialHandles;
        for (const Material* material : materials)
        {
            materialHandles.push_back(material->GetHandle());
        }

        mesh->SetDefaultMaterials(materialHandles);
    }

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

    std::string currentMTL;

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
        else if (type == "usemtl")
        {
            lineStream >> currentMTL;
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
            uv.y = 1.0f - uv.y;
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

            objInfo.mtlTofaces[currentMTL].push_back(face);
        }
    }

    return objInfo;
}

OBJ::MTLInfo OBJImporter::ParseMTL(const VirtualPath& mtlPath) const
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    const std::string fileText = fileSystem.ReadText(mtlPath);
    if (fileText.empty())
        return {};

    OBJ::MTLInfo mtlInfo = {};

    std::istringstream stream(fileText);

    bool hasCurrentMTL = false;
    OBJ::MTLData mtlData = {};

    std::string line;
    while (std::getline(stream, line))
    {
        std::istringstream lineStream(line);

        std::string type;
        lineStream >> type;

        if (type == "newmtl")
        {
            if (hasCurrentMTL)
            {
                mtlInfo.datas.push_back(mtlData);
                mtlData = {};
                hasCurrentMTL = false;
            }

            lineStream >> mtlData.mtlName;
            hasCurrentMTL = true;
        }
        else if (type == "Ns")
        {
            lineStream >> mtlData.shininess;
        }
        else if (type == "Ka")
        {
            lineStream >> mtlData.ambient.r >> mtlData.ambient.g >> mtlData.ambient.b;
        }
        else if (type == "Ks")
        {
            lineStream >> mtlData.specular.r >> mtlData.specular.g >> mtlData.specular.b;
        }
        else if (type == "Ke")
        {
            lineStream >> mtlData.emissive.r >> mtlData.emissive.g >> mtlData.emissive.b;
        }
        else if (type == "Ni")
        {
            lineStream >> mtlData.refractiveIndex;
        }
        else if (type == "d")
        {
            lineStream >> mtlData.dissolve;
        }
        else if (type == "illum")
        {
            lineStream >> mtlData.illuminationModel;
        }
        else if (type == "map_Kd")
        {
            lineStream >> mtlData.diffuseTexturePath;
        }
    }

    if (hasCurrentMTL)
    {
        mtlInfo.datas.push_back(mtlData);
    }

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
