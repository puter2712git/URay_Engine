#pragma once

#include "Engine/Asset/AssetMetadata.h"

#include <vector>

namespace URay
{

class Asset;
class VirtualPath;

struct AssetEntry
{
    Asset* asset;
    AssetMetadata metadata = {};
};

class Importer
{
public:
    virtual ~Importer() = default;

public:
    virtual Asset* Import(const VirtualPath& sourcePath) = 0;
    virtual AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const = 0;
    virtual std::vector<UUID> CollectDependencies(const VirtualPath& sourcePath) const = 0;

    virtual bool CanImport(const std::string& extension) const = 0;
};

} // namespace URay
