#pragma once

#include "Engine/Asset/Importer/Importer.h"

#include <vector>

namespace URay
{

class VirtualPath;
class Texture;

class TextureImporter final : public Importer
{
public:
    TextureImporter();
    ~TextureImporter() override;

public:
    Asset* Import(const VirtualPath& sourcePath) override;
    AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const;
    std::vector<UUID> CollectDependencies(const VirtualPath& sourcePath) const override;

    bool CanImport(const std::string& extension) const override;

private:
    Texture* LoadTexture(const VirtualPath& sourcePath, const AssetMetadata& metadata) const;
};

} // namespace URay
