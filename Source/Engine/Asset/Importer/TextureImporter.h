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
    void Import(const VirtualPath& sourcePath) override;

    AssetMetadata CreateMetadata(const VirtualPath& sourcePath) const;

    bool CanImport(const std::string& extension) const override;
};

} // namespace URay
