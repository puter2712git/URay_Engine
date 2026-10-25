#pragma once
#include <Core/Type/Types.h>

#include <Engine/Asset/Metadata/AssetMetadata.h>

#include <memory>
#include <vector>

namespace URay
{

class Asset;
class AssetSystem;
class Importer;
class VirtualFileSystem;
class VirtualPath;
struct ImportRequest;

class AssetPipeline
{
public:
    AssetPipeline(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~AssetPipeline();

public:
    bool CanImport(const VirtualPath& sourcePath) const;

    bool Execute(const ImportRequest& request) const;

private:
    Importer* FindImporter(const String& extension) const;

private:
    AssetSystem& assetSystem;
    VirtualFileSystem& fileSystem;

    std::vector<std::unique_ptr<Importer>> importers;
};

} // namespace URay
