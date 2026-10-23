#pragma once

#include <Engine/Asset/Importer/Importer.h>

namespace URay
{

class VirtualFileSystem;
class VirtualPath;
struct MaterialData;

class MaterialImporter final : public Importer
{
public:
    MaterialImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~MaterialImporter() override;

public:
    ImportResult Import(const VirtualPath& sourcePath) override;
    bool CanImport(const std::string& extension) const override;

private:
    bool ParseMaterial(MaterialData& outMaterial, const VirtualPath& sourcePath) const;
};

} // namespace URay
