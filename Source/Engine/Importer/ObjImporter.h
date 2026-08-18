#pragma once

#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"

#include <string>
#include <vector>

namespace URay
{

class Mesh;
class Material;
class MaterialManager;
class Texture;

class VirtualFilesystem;
class VirtualPath;

class ObjImporter
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

        // VirtualPath diffuseTexturePath = {};
        // VirtualPath alphaTexturePath = {};
    };

    struct ImportResult
    {
        Mesh* mesh = nullptr;
        std::vector<Material*> materials;
        std::vector<Texture*> textures;
    };

public:
    ImportResult Import(const VirtualPath& filePath);

private:
    void Reset();

    void ParseObj(const VirtualPath& objPath);
    Face ParseFace(const std::string& line);

    void ParseMtl(const VirtualPath& mtlPath);

    ObjIndex ParseObjIndex(const std::string& token);

private:
    VirtualFilesystem& filesystem;

    std::vector<Vector3> positions;
    std::vector<Vector2> uvs;
    std::vector<Vector3> normals;
    std::vector<Face> faces;

    std::string mtllib;
    std::vector<MtlInfo> mtlInfos;
};

} // namespace URay
