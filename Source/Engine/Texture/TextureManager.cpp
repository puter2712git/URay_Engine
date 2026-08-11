#include "TextureManager.h"

#include "TextureAsset.h"

#include "Core/File/VirtualFilesystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

namespace URay
{

TextureManager::TextureManager(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

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

TextureAsset* TextureManager::LoadTextureAsset(const std::string& key, const VirtualPath& virtualPath)
{
    if (!filesystem.Exists(virtualPath))
        return nullptr;

    std::string physicalPathStr = filesystem.ResolveToPhysicalPath(virtualPath).string();

    int width, height, channels;
    stbi_uc* data = stbi_load(physicalPathStr.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
    {
        std::cout << physicalPathStr << "\n";
        return nullptr;
    }

    TextureAsset* asset = new TextureAsset(physicalPathStr, width, height, channels);
    textures.insert({ key, asset });

    asset->SetName(key);

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
