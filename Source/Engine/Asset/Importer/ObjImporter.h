#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Type/Types.h>
#include <Engine/Asset/Importer/Importer.h>
#include <Engine/Asset/Importer/OBJInfo.h>
#include <Render/RHI/Vertex/Vertex.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace URay
{

class Material;

class VirtualFileSystem;

class OBJImporter : public Importer
{
public:
    OBJImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~OBJImporter();

public:
    DependencyResult DiscoverDependencies(const VirtualPath& sourcePath) override;
    ImportResult Import(const VirtualPath& sourcePath) override;
    bool CanImport(const std::string& extension) const override;

    // private:
    // void Assemble(const OBJ::OBJInfo& objInfo, const OBJ::MTLInfo& mtlInfo, const AssetMetadata& metadata) const;
    // std::vector<Material*> CreateMaterials(const OBJ::MTLInfo& mtlInfo, const VirtualPath& directory) const;
    // void CreateMesh(const OBJ::OBJInfo& objInfo, const std::vector<Material*>& materialHandles, const AssetMetadata& metadata) const;

    OBJ::OBJInfo ParseOBJ(const VirtualPath& objPath) const;
    OBJ::MTLInfo ParseMTL(const VirtualPath& mtlPath) const;

    OBJ::OBJIndex ParseOBJIndex(const std::string& vertexString) const;

    uint32 GetOrCreateVertex(
        const OBJ::OBJInfo& objInfo,
        const OBJ::OBJIndex& objIndex,
        std::vector<Render::VertexPNT>& verticesRef,
        std::unordered_map<OBJ::OBJIndex, uint32, OBJ::OBJIndexHash>& vertexMap) const;
};

} // namespace URay
