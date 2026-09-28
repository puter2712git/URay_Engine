#pragma once

#include "Engine/Asset/AssetMetadata.h"

#include <vector>

namespace URay
{

class VirtualPath;

class Importer
{
public:
    virtual ~Importer() = default;

public:
    virtual void Import(const VirtualPath& sourcePath) = 0;

    virtual AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const = 0;

    virtual bool CanImport(const std::string& extension) const = 0;
};

} // namespace URay
