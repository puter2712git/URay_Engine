#include "TextureImporter.h"

#include "Engine/Texture/Texture.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/Log.h"

#include <stb/stb_image.h>

namespace URay
{

TextureImporter::TextureImporter(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

Texture* TextureImporter::Import(const VirtualPath& path) const
{
    if (!filesystem.Exists(path))
        return nullptr;

    std::string physicalPath = filesystem.ResolveToPhysicalPath(path);

    int width, height, channels;
    stbi_uc* data = stbi_load(physicalPath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
    {
        Logger::Log("Failed to parse texture.");
        return nullptr;
    }

    Texture* newTexture = new Texture(physicalPath, width, height, channels);
    newTexture->SetName(physicalPath);

    return newTexture;
}

} // namespace URay
