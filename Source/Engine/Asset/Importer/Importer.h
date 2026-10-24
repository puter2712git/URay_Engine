#pragma once

#include <Engine/Asset/Importer/ImportResult.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>

#include <vector>

namespace URay
{

class AssetSystem;
class Class;
class VirtualFileSystem;
class VirtualPath;
struct ImportRequest;

class Importer
{
public:
    Importer(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    virtual ~Importer() = default;

public:
    virtual DependencyResult DiscoverDependencies(const ImportRequest& request) { return DependencyResult::Success(); }
    virtual ImportResult Import(const ImportRequest& request) = 0;
    virtual bool CanImport(const std::string& extension) const = 0;

protected:
    AssetMetadata LoadOrCreateMetadata(const VirtualPath& sourcePath, const VirtualPath& metadataPath, Class* assetClass) const;

protected:
    AssetSystem& assetSystem;
    VirtualFileSystem& fileSystem;
};

} // namespace URay
