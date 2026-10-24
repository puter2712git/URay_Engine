#include "TextureImporter.h"

#include <Core/File/VirtualFileSystem.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Importer/ImportRequest.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>
#include <Engine/Asset/Metadata/AssetMetadataSerializer.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Asset/Texture/TextureData.h>
#include <Engine/Asset/Texture/TextureSerializer.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <yaml-cpp/yaml.h>

namespace URay
{

TextureImporter::TextureImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

TextureImporter::~TextureImporter() = default;

ImportResult TextureImporter::Import(const ImportRequest& request)
{
    const VirtualPath assetPath = request.outputDirectory.Join(request.sourcePath.GetStem() + ".urtex");
    const VirtualPath metadataPath = assetSystem.GetMetadataPath(assetPath);

    TextureData assetData;
    if (!DecodeTexture(assetData, request.sourcePath))
        return ImportResult::Failure("Failed to decode texture: " + request.sourcePath.ToString());

    TextureSerializer& serializer = assetSystem.GetTextureSerializer();
    if (!serializer.Save(assetData, assetPath))
        return ImportResult::Failure("Failed to save texture: " + assetPath.ToString());

    AssetMetadata metadata = LoadOrCreateMetadata(request.sourcePath, metadataPath, Texture::StaticClass());

    ImportResult result = ImportResult::Success();
    result.metadatas.push_back(metadata);
    return result;
}

bool TextureImporter::CanImport(const std::string& extension) const
{
    return extension == ".png" || extension == ".jpg";
}

bool TextureImporter::DecodeTexture(TextureData& outTexture, const VirtualPath& sourcePath) const
{
    int32 width, height, channels;
    std::vector<uint8> fileBytes = fileSystem.ReadBinary(sourcePath);

    stbi_uc* data = stbi_load_from_memory(
        fileBytes.data(),
        static_cast<int>(fileBytes.size()),
        &width, &height, &channels,
        STBI_rgb_alpha);

    if (!data)
        return false;

    outTexture.width = width;
    outTexture.height = height;
    outTexture.format = TexturePixelFormat::RGBA8;
    outTexture.pixels = std::vector<uint8>(data, data + width * height * 4);

    stbi_image_free(data);

    return true;
}

} // namespace URay
