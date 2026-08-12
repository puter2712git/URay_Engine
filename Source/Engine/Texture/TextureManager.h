#pragma once

#include <string>
#include <unordered_map>

namespace URay
{

class VirtualFilesystem;
class VirtualPath;
class Texture;

class TextureManager
{
public:
    TextureManager(VirtualFilesystem& filesystem);
    ~TextureManager();

public:
    Texture* LoadTexture(const std::string& key, const VirtualPath& virtualPath);
    Texture* GetTexture(const std::string& key) const;
    const std::unordered_map<std::string, Texture*>& GetTextures() const { return textures; }

private:
    VirtualFilesystem& filesystem;

    std::unordered_map<std::string, Texture*> textures;
};

} // namespace URay
