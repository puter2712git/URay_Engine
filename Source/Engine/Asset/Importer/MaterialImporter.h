#pragma once

#include "Engine/Asset/Importer/Importer.h"
#include "Engine/Asset/Material/MaterialCookData.h"
#include "Engine/Asset/Material/MaterialSerializer.h"

namespace URay
{

class VirtualFilesystem;
class VirtualPath;

class MaterialImporter final : public Importer
{
public:
    explicit MaterialImporter(VirtualFilesystem& filesystem);

    ImportResult Import(const VirtualPath& path, ImportContext& context) override;
    bool CanImport(const std::string& extension) override;

private:
    bool LoadSource(const VirtualPath& path, MaterialCookData& data) const;

private:
    VirtualFilesystem& filesystem;
    MaterialSerializer serializer;
};

} // namespace URay
