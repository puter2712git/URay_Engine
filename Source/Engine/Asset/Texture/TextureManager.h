#pragma once

#include <string>
#include <unordered_map>

namespace URay
{

class TextureAsset;

class TextureManager
{
public:
    ~TextureManager();

public:
    TextureAsset* LoadTextureAsset(const std::string& key, const std::string& filePath);
    TextureAsset* GetTexture(const std::string& key) const;
    const std::unordered_map<std::string, TextureAsset*>& GetTextures() const { return textures; }

private:
    std::unordered_map<std::string, TextureAsset*> textures;
};

} // namespace URay
