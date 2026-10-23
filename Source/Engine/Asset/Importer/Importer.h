#pragma once

#include <Engine/Asset/AssetMetadata.h>

#include <vector>

namespace URay
{

class AssetSystem;
class VirtualFileSystem;
class VirtualPath;

class Importer
{
public:
    Importer(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    virtual ~Importer() = default;

public:
    virtual void Import(const VirtualPath& sourcePath) = 0;
    virtual bool CanImport(const std::string& extension) const = 0;

protected:
    AssetMetadata LoadOrCreateMetadata(const VirtualPath& sourcePath, const VirtualPath& metadataPath, AssetType assetType) const;

protected:
    AssetSystem& assetSystem;
    VirtualFileSystem& fileSystem;
};

} // namespace URay
