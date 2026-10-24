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

class VirtualFileSystem;

struct ImportedMaterials
{
    std::vector<AssetHandle> handles;
    std::unordered_map<std::string, uint32> indicesByName;
};

class OBJImporter : public Importer
{
public:
    OBJImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~OBJImporter();

public:
    DependencyResult DiscoverDependencies(const ImportRequest& request) override;
    ImportResult Import(const ImportRequest& request) override;
    bool CanImport(const std::string& extension) const override;

private:
    bool ImportMaterials(const ImportRequest& request, const OBJ::MTLInfo& mtlInfo, ImportResult& outResult, ImportedMaterials& outMaterials) const;
    bool ImportMesh(const ImportRequest& request, const OBJ::OBJInfo& objInfo, const ImportedMaterials& materials, ImportResult& outResult) const;

    void AssignFallbackMaterials(const OBJ::OBJInfo& objInfo, ImportedMaterials& materials) const;

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
