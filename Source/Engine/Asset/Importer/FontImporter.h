#pragma once

#include <Engine/Asset/Importer/Importer.h>

namespace URay
{

class VirtualPath;
struct FontData;

class FontImporter : public Importer
{
public:
    FontImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~FontImporter() override;

public:
    ImportResult Import(const VirtualPath& sourcePath) override;
    bool CanImport(const std::string& extension) const override;

private:
    bool DecodeFont(FontData& outFont, const VirtualPath& sourcePath) const;
};

} // namespace URay
