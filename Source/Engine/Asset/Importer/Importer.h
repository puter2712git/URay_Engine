#pragma once

#include "Engine/Asset/AssetMetadata.h"

#include <vector>

namespace URay
{

class Asset;
class VirtualPath;
class ImportContext;

struct AssetEntry
{
    Asset* asset;
    AssetMetadata metadata = {};
};

struct ImportResult
{
    std::vector<AssetEntry> entries;
};

class Importer
{
public:
    virtual ~Importer() = default;

public:
    virtual ImportResult Import(const VirtualPath& path, ImportContext& context) = 0;

    virtual bool CanImport(const std::string& extension) = 0;
};

} // namespace URay
