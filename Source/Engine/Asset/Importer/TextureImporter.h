#pragma once

#include "Engine/Asset/Importer/ImportResult.h"

#include <vector>

namespace URay
{

class VirtualFilesystem;
class VirtualPath;

class Texture;

class TextureImporter
{
public:
    TextureImporter(VirtualFilesystem& filesystem);
    ~TextureImporter();

public:
    std::vector<ImportResult> Import(const VirtualPath& path);

private:
    VirtualFilesystem& filesystem;
};

} // namespace URay
