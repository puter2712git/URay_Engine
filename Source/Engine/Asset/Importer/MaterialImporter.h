#pragma once

#include "Engine/Asset/Importer/Importer.h"

namespace URay
{

class VirtualFileSystem;
class VirtualPath;

class MaterialImporter final : public Importer
{
public:
    MaterialImporter();
    ~MaterialImporter() override;

public:
    void Import(const VirtualPath& sourcePath) override;

    AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const override;

    bool CanImport(const std::string& extension) const override;

private:
    bool LoadSource(const VirtualPath& path) const;
};

} // namespace URay
