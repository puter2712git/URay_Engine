#pragma once

#include <Engine/Asset/Importer/Importer.h>

#include <vector>

namespace URay
{

class AssetSystem;
class VirtualFileSystem;
class VirtualPath;
class Texture;
struct TextureData;

class TextureImporter final : public Importer
{
public:
    TextureImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem);
    ~TextureImporter() override;

public:
    void Import(const VirtualPath& sourcePath) override;
    bool CanImport(const std::string& extension) const override;

private:
    bool DecodeTexture(TextureData& outTexture, const VirtualPath& sourcePath) const;
};

} // namespace URay
