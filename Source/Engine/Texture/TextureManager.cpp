#include "TextureManager.h"

#include "TextureAsset.h"

#include "Core/File/FileIO.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace URay
{

TextureManager::~TextureManager()
{
    for (auto& [key, texture] : textures)
    {
        if (texture)
        {
            delete texture;
            texture = nullptr;
        }
    }

    textures.clear();
}

TextureAsset* TextureManager::LoadTextureAsset(const std::string& key, const std::string& filePath)
{
    if (!FileIO::Exists(filePath))
        return nullptr;

    int width, height, channels;
    stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
        return nullptr;

    TextureAsset* asset = new TextureAsset(filePath, width, height, channels);
    textures.insert({ key, asset });

    return asset;
}

TextureAsset* TextureManager::GetTexture(const std::string& key) const
{
    auto it = textures.find(key);
    if (it == textures.end())
        return nullptr;

    return it->second;
}

} // namespace URay
