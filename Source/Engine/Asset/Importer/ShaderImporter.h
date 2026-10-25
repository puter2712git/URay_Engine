#pragma once
#include <Core/Type/Types.h>

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
    ImportResult Import(const ImportRequest& request) override;
    bool CanImport(const String& extension) const override;
};

} // namespace URay
