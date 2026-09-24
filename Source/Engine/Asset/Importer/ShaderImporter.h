#pragma once

#include "Engine/Asset/Importer/Importer.h"

namespace URay
{

class VirtualFilesystem;
class VirtualPath;

class ShaderImporter final : public Importer
{
public:
    ShaderImporter();
    ~ShaderImporter() override;

public:
    Asset* Import(const VirtualPath& sourcePath) override;
    AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const override;
    std::vector<UUID> CollectDependencies(const VirtualPath& sourcePath) const override;

    bool CanImport(const std::string& extension) const override;
};

} // namespace URay
