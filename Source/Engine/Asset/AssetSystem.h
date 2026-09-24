#pragma once

#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Asset/DefaultAssets.h"

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

class VirtualFilesystem;
class VirtualPath;
class Asset;
class AssetFactory;
class Importer;
class Engine;

class AssetSystem
{
public:
    AssetSystem();
    ~AssetSystem();

public:
    bool Initialize(const std::string& enginePath, const std::string& projectPath);
    bool CreateDefaultAssets();
    bool LoadAssets(const VirtualPath& sourceDir);

    void Finalize();

    Asset* Import(const VirtualPath& path);

    VirtualPath GetImportAssetPath(const VirtualPath& sourcePath) const;

    std::optional<AssetMetadata> FindAssetMetadataByUUID(const UUID& uuid) const;
    std::optional<UUID> FindUUIDBySourcePath(const std::string& sourcePath) const;

    template <typename T>
    T* Find(const UUID& uuid) const
    {
        const auto it = assets.find(uuid);

        if (it == assets.end())
            return nullptr;

        return Cast<T>(it->second);
    }

    template <typename T>
    std::vector<T*> FindAssets() const
    {
        std::vector<T*> ret;

        for (auto& [uuid, asset] : assets)
        {
            if (T* obj = Cast<T>(asset))
            {
                ret.push_back(obj);
            }
        }

        return ret;
    }

    VirtualFilesystem& GetFilesystem() const { return *filesystem; }

    AssetFactory& GetAssetFactory() const { return *factory; } // TODO: Remove this getter.

    const DefaultAssets& GetDefaultAssets() const { return defaultAssets; }

private:
    std::vector<AssetMetadata> ScanAssets();
    std::vector<AssetMetadata> ScanAssetsRecursive(const VirtualPath& path);
    AssetMetadata LoadAssetMetadata(const VirtualPath& path);

    std::vector<AssetMetadata> SortByDependency(const std::vector<AssetMetadata>& metadatas) const;

    void ImportAll(const std::vector<AssetMetadata>& sortedMetadatas);

    Importer* GetImporterByExtension(const std::string& extension) const;

private:
    std::unique_ptr<VirtualFilesystem> filesystem = nullptr;

    std::vector<std::unique_ptr<Importer>> importers;

    std::unique_ptr<AssetFactory> factory = nullptr;

    std::unordered_map<UUID, Asset*, UUIDHash> assets;
    std::unordered_map<UUID, AssetMetadata, UUIDHash> assetMetadatas;

    std::unordered_map<std::string, UUID> sourceAssets;

    DefaultAssets defaultAssets = {};
};

} // namespace URay
