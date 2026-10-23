#pragma once

#include <Engine/Asset/Importer/Importer.h>

namespace URay
{

class VirtualFileSystem;
class VirtualPath;

class MaterialImporter final : public Importer
{
public:
    MaterialImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~MaterialImporter() override;

public:
    void Import(const VirtualPath& sourcePath) override;
    bool CanImport(const std::string& extension) const override;

private:
    bool LoadSource(const VirtualPath& path) const;
};

} // namespace URay
