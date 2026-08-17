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

TextureImporter::~TextureImporter() = default;

ImportResult TextureImporter::Import(const VirtualPath& path)
{
    ImportResult result = {};

    if (!filesystem.Exists(path))
    {
        Value<Object*> failure = Value<Object*>(nullptr);
        failure.isValid = false;
        failure.errorMsg = "Failed to find path: " + path.ToString();

        result.objects.push_back(failure);
        return result;
    }

    std::string physicalPath = filesystem.ResolveToPhysicalPath(path).string();

    int width, height, channels;
    stbi_uc* data = stbi_load(physicalPath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
    {
        Value<Object*> failure = Value<Object*>(nullptr);
        failure.isValid = false;
        failure.errorMsg = "Failed to parse texture: " + physicalPath;

        result.objects.push_back(failure);
        return result;
    }

    Texture* newTexture = new Texture(physicalPath, width, height, channels);
    newTexture->SetName(physicalPath);

    Value<Object*> textureObject = Value<Object*>(newTexture);
    textureObject.isValid = true;

    result.objects.push_back(textureObject);

    return result;
}

} // namespace URay
