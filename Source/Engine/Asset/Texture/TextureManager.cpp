#include "TextureManager.h"

#include "Texture.h"

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

Texture* TextureManager::LoadTexture(const std::string& key, const VirtualPath& virtualPath)
{
    if (!filesystem.Exists(virtualPath))
        return nullptr;

    std::vector<uint8_t> fileBytes = filesystem.ReadBinary(virtualPath);

    int width, height, channels;
    stbi_uc* data = stbi_load_from_memory(fileBytes.data(), static_cast<int>(fileBytes.size()),
                                          &width, &height, &channels,
                                          STBI_rgb_alpha);

    if (!data)
        return nullptr;

    std::vector<uint8_t> pixels(data, data + width * height * 4);

    Texture* texture = new Texture(virtualPath.ToString(), width, height, channels, pixels);
    texture->SetName(key);
    textures.insert({ key, texture });

    stbi_image_free(data);

    return texture;
}

Texture* TextureManager::GetTexture(const std::string& key) const
{
    auto it = textures.find(key);
    if (it == textures.end())
        return nullptr;

    return it->second;
}

} // namespace URay
