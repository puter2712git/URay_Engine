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
    ~TextureImporter();

public:
    Texture* Import(const VirtualPath& path);

private:
    VirtualFilesystem& filesystem;
};

} // namespace URay
