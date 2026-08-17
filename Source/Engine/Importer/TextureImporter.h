#pragma once

#include "Engine/Importer/Importer.h"

namespace URay
{

class VirtualFilesystem;
class VirtualPath;

class Texture;

class TextureImporter : public Importer
{
public:
    TextureImporter(VirtualFilesystem& filesystem);
    ~TextureImporter() override;

public:
    ImportResult Import(const VirtualPath& path) override;

private:
    VirtualFilesystem& filesystem;
};

} // namespace URay
