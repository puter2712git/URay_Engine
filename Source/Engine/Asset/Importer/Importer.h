#pragma once

#include "Engine/Asset/AssetMetadata.h"

#include <vector>

namespace URay
{

class Object;
class VirtualPath;

struct AssetEntry
{
    Object* asset;
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
    virtual ImportResult Import(const VirtualPath& path) = 0;

    virtual bool CanImport(const std::string& extension) = 0;
};

} // namespace URay
