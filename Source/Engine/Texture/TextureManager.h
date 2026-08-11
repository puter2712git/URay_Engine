#pragma once

#include <string>
#include <unordered_map>

namespace URay
{

class VirtualFilesystem;
class VirtualPath;
class TextureAsset;

class TextureManager
{
public:
    TextureManager(VirtualFilesystem& filesystem);
    ~TextureManager();

public:
    TextureAsset* LoadTextureAsset(const std::string& key, const VirtualPath& virtualPath);
    TextureAsset* GetTexture(const std::string& key) const;
    const std::unordered_map<std::string, TextureAsset*>& GetTextures() const { return textures; }

private:
    VirtualFilesystem& filesystem;

    std::unordered_map<std::string, TextureAsset*> textures;
};

} // namespace URay
