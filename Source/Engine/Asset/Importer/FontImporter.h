#pragma once

#include <Engine/Asset/Importer/Importer.h>

namespace URay
{

class FontImporter : public Importer
{
public:
    FontImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~FontImporter() override;

public:
    void Import(const VirtualPath& sourcePath) override;
    bool CanImport(const std::string& extension) const override;
};

} // namespace URay
