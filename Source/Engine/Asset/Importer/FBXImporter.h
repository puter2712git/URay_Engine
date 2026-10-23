#pragma once

#include <Engine/Asset/AssetHandle.h>
#include <Engine/Asset/Importer/FBXInfo.h>
#include <Engine/Asset/Importer/Importer.h>

#include <vector>

struct ufbx_mesh;
struct ufbx_scene;

namespace URay
{

class FBXImporter : public Importer
{
public:
    FBXImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~FBXImporter() override;

public:
    void Import(const VirtualPath& sourcePath) override;
    bool CanImport(const std::string& extension) const override;

private:
    FBX::FBXInfo ParseFBXInfo(const ufbx_scene& sourceScene) const;

    bool CreateStaticMesh(const ufbx_scene& sourceScene, const AssetMetadata& metadata) const;
    bool CreateSkinnedMesh(const ufbx_scene& sourceScene, const FBX::FBXInfo& fbxInfo, const AssetMetadata& metadata) const;

    std::vector<AssetHandle> CreateMaterials(const ufbx_mesh& sourceMesh, const AssetMetadata& metadata) const;
};

} // namespace URay
