#pragma once

#include "Core/File/VirtualPath.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"

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

namespace RHI
{
class Shader;
}

class ObjImporter
{
public:
    ObjImporter(VirtualFilesystem& filesystem,
                MeshManager& meshManager,
                TextureManager& textureManager,
                MaterialManager& materialManager,
                RHI::Shader* meshShader);

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
        std::unordered_map<std::string, uint32_t> slots;
    };

public:
    Mesh* Import(const VirtualPath& filePath);

private:
    void Reset();

    void ParseObj(const VirtualPath& objPath);
    Face ParseFace(const std::string& line);
    ObjIndex ParseObjIndex(const std::string& token);

    void ParseMtl(const VirtualPath& mtlPath);
    MaterialImportResult CreateMaterials();

private:
    VirtualFilesystem& filesystem;
    MeshManager& meshManager;
    TextureManager& textureManager;
    MaterialManager& materialManager;

    RHI::Shader* meshShader = nullptr;

    std::vector<Vector3> positions;
    std::vector<Vector2> uvs;
    std::vector<Vector3> normals;
    std::vector<Face> faces;

    std::string mtllib;
    std::vector<MtlInfo> mtlInfos;
};

} // namespace URay
