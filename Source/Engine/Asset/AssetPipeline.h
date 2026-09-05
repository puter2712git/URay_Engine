#pragma once

#include "Engine/Asset/Importer/Importer.h"

#include <memory>
#include <vector>

namespace URay
{

class AssetSystem;
class VirtualFilesystem;
class Importer;

class AssetPipeline
{
public:
    AssetPipeline(AssetSystem& assetSystem);
    ~AssetPipeline();

public:
    bool Initialize();
    void Finalize();

    ImportResult Import(const VirtualPath& path);

private:
    Importer* FindImporter(const std::string& extension) const;

private:
    AssetSystem& assetSystem;
    VirtualFilesystem& filesystem;

    std::vector<std::unique_ptr<Importer>> importers;
};

} // namespace URay
