#pragma once

#include <Engine/Asset/AssetMetadata.h>

#include <memory>
#include <string>
#include <vector>

namespace URay
{

class Asset;
class AssetSystem;
class Importer;
class VirtualFileSystem;
class VirtualPath;

class AssetPipeline
{
public:
    AssetPipeline(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~AssetPipeline();

public:
    void Execute(const VirtualPath& path) const;

private:
    Importer* FindImporter(const std::string& extension) const;

private:
    AssetSystem& assetSystem;
    VirtualFileSystem& fileSystem;

    std::vector<std::unique_ptr<Importer>> importers;
};

} // namespace URay
