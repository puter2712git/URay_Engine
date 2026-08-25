#include "TextureImporter.h"

#include "Engine/Asset/Texture/Texture.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/Log.h"

#include <stb/stb_image.h>

namespace URay
{

TextureImporter::TextureImporter(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

TextureImporter::~TextureImporter() = default;

Texture* TextureImporter::Import(const VirtualPath& path)
{
    return nullptr;
}

} // namespace URay
