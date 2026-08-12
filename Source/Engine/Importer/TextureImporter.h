#pragma once

namespace URay
{

class VirtualFilesystem;
class VirtualPath;

class Texture;

class TextureImporter
{
public:
    TextureImporter(VirtualFilesystem& filesystem);

public:
    Texture* Import(const VirtualPath& path) const;

private:
    VirtualFilesystem& filesystem;
};

} // namespace URay
