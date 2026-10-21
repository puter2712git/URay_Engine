#pragma once

#include <Engine/Asset/AssetHandle.h>
#include <Engine/Asset/Importer/Importer.h>

#include <vector>

struct ufbx_mesh;
struct ufbx_scene;

namespace URay
{

class FBXImporter : public Importer
{
public:
    void Import(const VirtualPath& sourcePath) override;

    AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const override;

    bool CanImport(const std::string& extension) const override;

private:
    bool CreateStaticMesh(const ufbx_scene& sourceScene, const AssetMetadata& metadata) const;

    std::vector<AssetHandle> CreateMaterials(const ufbx_mesh& sourceMesh, const AssetMetadata& metadata) const;
};

} // namespace URay
