#pragma once

#include "Engine/Asset/Importer/Importer.h"

namespace URay
{

class VirtualFileSystem;
class VirtualPath;

class ShaderImporter final : public Importer
{
public:
    ShaderImporter();
    ~ShaderImporter() override;

public:
    void Import(const VirtualPath& sourcePath) override;

    AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const override;

    bool CanImport(const std::string& extension) const override;
};

} // namespace URay
