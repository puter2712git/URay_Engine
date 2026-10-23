#pragma once

#include <Engine/Asset/Importer/Importer.h>

namespace URay
{

class VirtualFileSystem;
class VirtualPath;

class ShaderImporter final : public Importer
{
public:
    ShaderImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~ShaderImporter() override;

public:
    ImportResult Import(const VirtualPath& sourcePath) override;
    bool CanImport(const std::string& extension) const override;
};

} // namespace URay
