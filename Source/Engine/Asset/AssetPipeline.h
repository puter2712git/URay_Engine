#pragma once

namespace URay
{

class VirtualFilesystem;
class VirtualPath;

class TextureImporter;

class AssetPipeline
{
public:
    AssetPipeline(VirtualFilesystem& filesystem);
    ~AssetPipeline();

public:
    void Import(const VirtualPath& path);

private:
    VirtualFilesystem& filesystem;

    TextureImporter* textureImporter = nullptr;
};

} // namespace URay
