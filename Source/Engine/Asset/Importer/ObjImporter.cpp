#include "ObjImporter.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Core/Type/Types.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Material/MaterialData.h>
#include <Engine/Asset/Material/MaterialSerializer.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Mesh/MeshData.h>
#include <Engine/Asset/Mesh/MeshSerializer.h>
#include <Engine/Asset/Metadata/AssetMetadataSerializer.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>

#include <iostream>
#include <sstream>
#include <unordered_map>

namespace URay
{

OBJImporter::OBJImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

OBJImporter::~OBJImporter() = default;

DependencyResult OBJImporter::DiscoverDependencies(const VirtualPath& sourcePath)
{
    OBJ::OBJInfo objInfo = ParseOBJ(sourcePath);

    const VirtualPath mtlPath = sourcePath.GetDirectory().Join(objInfo.mtllib);
    OBJ::MTLInfo mtlInfo = ParseMTL(mtlPath);

    DependencyResult result = DependencyResult::Success();

    for (const OBJ::MTLData& material : mtlInfo.datas)
    {
        if (!material.diffuseTexturePath.empty())
        {
            result.sourcePaths.push_back(sourcePath.GetDirectory().Join(material.diffuseTexturePath));
        }
    }

    return result;
}

ImportResult OBJImporter::Import(const VirtualPath& sourcePath)
{
    OBJ::OBJInfo objInfo = ParseOBJ(sourcePath);

    const VirtualPath mtlPath = sourcePath.GetDirectory().Join(objInfo.mtllib);
    OBJ::MTLInfo mtlInfo = ParseMTL(mtlPath);

    ImportResult result = ImportResult::Success();

    ImportedMaterials importedMaterials;
    if (!ImportMaterials(sourcePath, mtlInfo, result, importedMaterials))
    {
        return ImportResult::Failure("Failed to import materials.");
    }

    if (!ImportMesh(sourcePath, objInfo, importedMaterials, result))
    {
        return ImportResult::Failure("Failed to import mesh.");
    }

    return result;
}

bool OBJImporter::CanImport(const std::string& extension) const
{
    return extension == ".obj";
}

bool OBJImporter::ImportMaterials(
    const VirtualPath& sourcePath,
    const OBJ::MTLInfo& mtlInfo,
    ImportResult& outResult,
    ImportedMaterials& outMaterials) const
{
    AssetMetadataSerializer& metadataSerializer = assetSystem.GetMetadataSerializer();
    MaterialSerializer& materialSerializer = assetSystem.GetMaterialSerializer();

    outMaterials = {};

    const VirtualPath directory = sourcePath.GetDirectory();

    for (const OBJ::MTLData& mtlData : mtlInfo.datas)
    {
        MaterialData materialData = {};
        materialData.shaderHandle = EngineAsset::MeshShader;

        if (!mtlData.diffuseTexturePath.empty())
        {
            const VirtualPath diffuseTexturePath = directory.Join(mtlData.diffuseTexturePath);
            const VirtualPath textureAssetPath = diffuseTexturePath.ReplaceExtension(".urtex");
            const VirtualPath textureMetadataPath = assetSystem.GetMetadataPath(textureAssetPath);

            AssetMetadata textureMetadata;
            if (!metadataSerializer.Load(textureMetadata, textureMetadataPath))
                return false;

            materialData.parameters.emplace_back(
                "diffuseColorTexture",
                MaterialParameterType::Texture2D,
                textureMetadata.handle);
        }

        const uint32 materialIndex = static_cast<uint32>(outMaterials.handles.size());

        const VirtualPath materialAssetPath = directory.Join(sourcePath.GetStem() + "_" + mtlData.mtlName + ".urmat");
        const VirtualPath materialMetadataPath = assetSystem.GetMetadataPath(materialAssetPath);

        AssetMetadata materialMetadata = LoadOrCreateMetadata(materialAssetPath, materialMetadataPath, Material::StaticClass());

        if (!materialSerializer.Save(materialData, materialAssetPath))
            return false;

        outMaterials.handles.push_back(materialMetadata.handle);
        outMaterials.indicesByName.emplace(mtlData.mtlName, materialIndex);

        outResult.metadatas.push_back(materialMetadata);
    }

    return true;
}

bool OBJImporter::ImportMesh(
    const VirtualPath& sourcePath,
    const OBJ::OBJInfo& objInfo,
    const ImportedMaterials& importedMaterials,
    ImportResult& outResult) const
{
    MeshSerializer& meshSerializer = assetSystem.GetMeshSerializer();

    std::vector<Render::VertexPNT> vertices;
    std::vector<uint32> indices;
    std::vector<MeshSection> sections;

    std::unordered_map<OBJ::OBJIndex, uint32, OBJ::OBJIndexHash> vertexMap;

    for (const auto& [mtlName, faces] : objInfo.mtlTofaces)
    {
        MeshSection section;
        section.indexOffset = static_cast<uint32>(indices.size());

        for (const OBJ::Face& face : faces)
        {
            for (size_t i = 1; i + 1 < face.indices.size(); ++i)
            {
                indices.push_back(GetOrCreateVertex(objInfo, face.indices[0], vertices, vertexMap));
                indices.push_back(GetOrCreateVertex(objInfo, face.indices[i], vertices, vertexMap));
                indices.push_back(GetOrCreateVertex(objInfo, face.indices[i + 1], vertices, vertexMap));
            }
        }

        section.indexCount = static_cast<uint32>(indices.size()) - section.indexOffset;

        const auto it = importedMaterials.indicesByName.find(mtlName);
        if (it == importedMaterials.indicesByName.end())
            return false;

        section.materialIndex = it->second;

        sections.push_back(section);
    }

    const VirtualPath meshAssetPath = sourcePath.ReplaceExtension(".urmesh");
    const VirtualPath meshMetadataPath = assetSystem.GetMetadataPath(meshAssetPath);

    AssetMetadata metadata = LoadOrCreateMetadata(meshAssetPath, meshMetadataPath, Mesh::StaticClass());

    MeshData meshData;
    meshData.vertices = std::move(vertices);
    meshData.indices = std::move(indices);
    meshData.sections = std::move(sections);
    meshData.defaultMaterials = importedMaterials.handles;

    if (!meshSerializer.Save(meshData, meshAssetPath))
        return false;

    outResult.metadatas.push_back(metadata);

    return true;
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
