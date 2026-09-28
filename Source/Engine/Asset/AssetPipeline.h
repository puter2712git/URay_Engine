#pragma once

#include "Engine/Asset/AssetMetadata.h"

#include <memory>
#include <string>
#include <vector>

namespace URay
{

class Asset;
class Importer;
class VirtualPath;

class AssetPipeline
{
public:
    AssetPipeline();
    ~AssetPipeline();

public:
    void Execute(const VirtualPath& path) const;

private:
    Importer* FindImporter(const std::string& extension) const;

private:
    std::vector<std::unique_ptr<Importer>> importers;
};

} // namespace URay
