#pragma once

#include "Engine/Asset/Importer/Importer.h"
#include "Engine/Asset/Mesh/MeshCookData.h"
#include "Engine/Asset/Mesh/MeshSerializer.h"

#include "Core/File/VirtualPath.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Type/Types.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace URay
{

class Mesh;
class Material;
class Texture;
class MeshManager;
class MaterialManager;
class TextureManager;

class VirtualFilesystem;

namespace Render
{
class Shader;
}

class ObjImporter : public Importer
{
public:
    ObjImporter(VirtualFilesystem& filesystem);

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
        std::string mtlName;
    };

    struct MtlInfo
    {
        std::string mtlName;

        float specularExponent = 0.0f;
        Vector3 diffuse = Vector3::One;
        Vector3 ambient = Vector3::One;
        Vector3 specular = Vector3::Zero;
        Vector3 emissive = Vector3::Zero;
        float refractiveIndex = 0.0f;
        int illum = 1;

        VirtualPath diffuseTexturePath = {};
        VirtualPath alphaTexturePath = {};
    };

    struct MaterialImportResult
    {
        std::vector<Material*> materials;
        std::unordered_map<std::string, uint32> slots;
        std::vector<MeshMaterialReference> references;
    };

public:
    ImportResult Import(const VirtualPath& path, ImportContext& context) override;

    bool CanImport(const std::string& extension) override;

private:
    void Reset();

    void ParseSource(const VirtualPath& path);
    MeshCookData BuildMeshCookData(
        const std::unordered_map<std::string, uint32>& materialSlots);
    MaterialImportResult LoadCookedMaterials(
        const std::vector<MeshMaterialReference>& references,
        ImportContext& context);

    void ParseObj(const VirtualPath& objPath);
    Face ParseFace(const std::string& line);
    ObjIndex ParseObjIndex(const std::string& token);

    void ParseMtl(const VirtualPath& mtlPath);
    MaterialImportResult CreateMaterials(const VirtualPath& meshPath, ImportContext& context);

private:
    VirtualFilesystem& filesystem;

    MeshSerializer serializer;

    std::vector<Vector3> positions;
    std::vector<Vector2> uvs;
    std::vector<Vector3> normals;
    std::vector<Face> faces;

    std::string mtllib;
    std::vector<MtlInfo> mtlInfos;
};

} // namespace URay
