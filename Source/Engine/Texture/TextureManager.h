#pragma once

#include <unordered_map>

namespace URay
{

class TextureAsset;

class TextureManager
{
public:
    TextureAsset* LoadTextureAsset(const std::string& key, const std::string& filePath);

private:
    std::unordered_map<std::string, TextureAsset*> textures;
};

} // namespace URay
