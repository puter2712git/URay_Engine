#pragma once

#include "Engine/Asset/AssetMetadata.h"

#include "Core/UUID.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace URay
{

class VirtualFileSystem;
class VirtualPath;
class Asset;
class AssetDatabase;
class AssetPipeline;
class Importer;
struct MeshInfo;

class AssetSystem
{
public:
    AssetSystem();
    ~AssetSystem();

public:
    bool Initialize(const std::string& enginePath, const std::string& projectPath);
    void Finalize();

    void ImportRecursive(const VirtualPath& path);

    void CreateDefaultAssets();

    VirtualPath GetImportAssetPath(const VirtualPath& sourcePath) const;

    VirtualFileSystem& GetFileSystem() const { return *fileSystem; }
    AssetDatabase& GetDatabase() const { return *database; }

private:
    void CreateDefaultMesh(const UUID& uuid, const std::string& name, const MeshInfo& meshInfo, const UUID& materialUUID);

private:
    std::unique_ptr<VirtualFileSystem> fileSystem = nullptr;

    std::unique_ptr<AssetDatabase> database = nullptr;
    std::unique_ptr<AssetPipeline> pipeline = nullptr;
};

} // namespace URay
