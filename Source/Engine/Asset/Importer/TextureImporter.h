#pragma once

#include "Engine/Asset/Importer/Importer.h"

#include <vector>

namespace URay
{

class VirtualFilesystem;
class VirtualPath;

class Texture;

class TextureImporter final : public Importer
{
public:
    TextureImporter(VirtualFilesystem& filesystem);
    ~TextureImporter() override;

public:
    ImportResult Import(const VirtualPath& path) override;

    bool CanImport(const std::string& extension) override;

private:
    Texture* LoadTexture(const VirtualPath& path, const AssetMetadata& metadata);

private:
    VirtualFilesystem& filesystem;
};

} // namespace URay
