#include "ObjImporter.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/DefaultAssets.h"
#include "Engine/Asset/Importer/ImportContext.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Type/Types.h"

#include <iostream>
#include <sstream>
#include <unordered_map>

namespace URay
{

using Render::VertexPNT;

ObjImporter::ObjImporter(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

ImportResult ObjImporter::Import(
    const VirtualPath& path,
    ImportContext& context)
{
    if (!filesystem.Exists(path))
        return ImportResult{};

    ImportResult result = {};

    VirtualPath importMetaPath = VirtualPath(
        "Asset://" + path.GetRelativePath() + ".meta");
    VirtualPath importAssetPath = VirtualPath(
        "Asset://" + path.GetRelativePath() + ".asset");

    AssetMetadata metadata = {};
    MeshCookData meshCookData = {};

    if (!filesystem.Exists(importMetaPath))
    {
        metadata.uuid = UUID::Generate();
        metadata.type = AssetType::Mesh;
        metadata.sourcePath = path;
        metadata.importPath = importAssetPath;

        YAML::Node metadataNode = metadata.Serialize();
        filesystem.WriteText(importMetaPath, YAML::Dump(metadataNode));
    }
    else
    {
        std::string metadataNodeString = filesystem.ReadText(importMetaPath);
        YAML::Node metadataNode = YAML::Load(metadataNodeString);

        metadata.Deserialize(metadataNode);
    }

    AssetSystem& assetSystem = context.GetAssetSystem();
    const DefaultAssets& defaultAssets = assetSystem.GetDefaultAssets();
    MaterialImportResult materialImportResult = {};

    if (!filesystem.Exists(importAssetPath))
    {
        ParseSource(path);
        materialImportResult = CreateMaterials(path, context);

        meshCookData = BuildMeshCookData(materialImportResult.slots);
        meshCookData.materials = materialImportResult.references;

        std::vector<uint8> serializedCookData = serializer.Serialize(meshCookData);
        filesystem.WriteBinary(importAssetPath, serializedCookData);
    }
    else
    {
        std::vector<uint8> bytes = filesystem.ReadBinary(importAssetPath);
        if (!serializer.Deserialize(bytes, meshCookData))
        {
            ParseSource(path);
            materialImportResult = CreateMaterials(path, context);
            meshCookData = BuildMeshCookData(materialImportResult.slots);
            meshCookData.materials = materialImportResult.references;
            filesystem.WriteBinary(importAssetPath, serializer.Serialize(meshCookData));
        }
        else
        {
            materialImportResult = LoadCookedMaterials(meshCookData.materials, context);
        }
    }

    if (materialImportResult.materials.empty())
    {
        materialImportResult.materials.push_back(defaultAssets.meshMaterial);
    }

    AssetFactory& assetFactory = assetSystem.GetAssetFactory();

    Mesh* mesh = assetFactory.CreateMesh(
        metadata,
        meshCookData.vertices,
        meshCookData.indices,
        meshCookData.sections,
        materialImportResult.materials);

    result.entries.push_back(AssetEntry{
        .asset = mesh,
        .metadata = metadata });

    return result;
}

bool ObjImporter::CanImport(const std::string& extension)
{
    if (extension == ".obj")
    {
        return true;
    }

    return false;
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

void ObjImporter::ParseSource(const VirtualPath& path)
{
    Reset();

    ParseObj(path);

    if (!mtllib.empty())
    {
        VirtualPath mtlPath = path.GetDirectory().Join(mtllib);
        ParseMtl(mtlPath);
    }
}

MeshCookData ObjImporter::BuildMeshCookData(
    const std::unordered_map<std::string, uint32>& materialSlots)
{

    std::vector<VertexPNT> vertices;
    std::vector<uint32> indices;
    std::unordered_map<ObjIndex, uint32, ObjIndexHash> vertexMap;

    auto AddVertex = [&](const ObjIndex& objIndex) -> uint32
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

        uint32 newIndex = static_cast<uint32>(vertices.size());
        vertices.push_back(vertex);
        vertexMap.insert({ objIndex, newIndex });

        return newIndex;
    };

    std::vector<MeshSection> sections;

    uint32 activeMaterialSlot = UINT32_MAX;
    MeshSection activeSection = {};

    for (const Face& face : faces)
    {
        uint32 materialSlot = 0;

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
                .indexOffset = static_cast<uint32>(indices.size()),
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

    MeshCookData data = {};
    data.vertices = vertices;
    data.indices = indices;
    data.sections = sections;

    return data;
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
        else if (type == "Kd")
        {
            ss >> mtlInfo.diffuse.x >> mtlInfo.diffuse.y >> mtlInfo.diffuse.z;
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

ObjImporter::MaterialImportResult ObjImporter::LoadCookedMaterials(
    const std::vector<MeshMaterialReference>& references,
    ImportContext& context)
{
    AssetSystem& assetSystem = context.GetAssetSystem();
    MaterialImportResult result = {};

    for (const MeshMaterialReference& reference : references)
    {
        if (!reference.sourcePath.ToString().empty())
            assetSystem.Import(reference.sourcePath);

        if (Material* material = assetSystem.Find<Material>(reference.uuid))
            result.materials.push_back(material);
        else
            result.materials.push_back(nullptr);
    }

    return result;
}

ObjImporter::MaterialImportResult ObjImporter::CreateMaterials(
    const VirtualPath& meshPath,
    ImportContext& context)
{
    AssetSystem& assetSystem = context.GetAssetSystem();

    MaterialImportResult result = {};

    for (const MtlInfo& info : mtlInfos)
    {
        const VirtualPath materialPath = meshPath.GetDirectory().Join(
            meshPath.GetStem() + "_" + info.mtlName + ".mat");

        if (!filesystem.Exists(materialPath))
        {
            YAML::Node materialNode;
            materialNode["Type"] = "Material";
            materialNode["Shader"] = "Mesh";
            materialNode["BaseColor"] = YAML::Load("[0, 0, 0, 1]");
            materialNode["BaseColor"][0] = info.diffuse.x;
            materialNode["BaseColor"][1] = info.diffuse.y;
            materialNode["BaseColor"][2] = info.diffuse.z;
            if (!info.diffuseTexturePath.ToString().empty())
                materialNode["BaseColorTexture"] = info.diffuseTexturePath.ToString();

            filesystem.WriteText(materialPath, YAML::Dump(materialNode));
        }

        const UUID materialUUID = assetSystem.Import(materialPath);
        Material* material = assetSystem.Find<Material>(materialUUID);

        uint32 slot = static_cast<uint32>(result.materials.size());

        result.materials.push_back(material);
        result.slots.insert({ info.mtlName, slot });
        result.references.push_back({
            .uuid = materialUUID,
            .sourcePath = materialPath });
    }

    return result;
}

} // namespace URay
