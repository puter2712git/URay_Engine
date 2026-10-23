#include "TextureImporter.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetMetadata.h>
#include <Engine/Asset/AssetMetadataSerializer.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Asset/Texture/TextureData.h>
#include <Engine/Asset/Texture/TextureSerializer.h>
#include <Engine/Engine.h>

#include <Core/File/VirtualFileSystem.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <yaml-cpp/yaml.h>

namespace URay
{

TextureImporter::TextureImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

TextureImporter::~TextureImporter() = default;

void TextureImporter::Import(const VirtualPath& sourcePath)
{
    const VirtualPath assetPath = sourcePath.ReplaceExtension(".urtex");
    const VirtualPath metadataPath = assetSystem.GetMetadataPath(assetPath);

    TextureImportData importData = {
        .sourcePath = sourcePath,
        .importerVersion = 1,
        .isSRGB = true,
        .generateMipmaps = false
    };

    TextureAssetData assetData;
    if (!DecodeTexture(assetData, sourcePath))
        return;

    TextureSerializer& serializer = assetSystem.GetTextureSerializer();

    serializer.Save(assetData, sourcePath.ReplaceExtension(".urtex"));

    AssetMetadata metadata = LoadOrCreateMetadata(sourcePath, metadataPath, AssetType::Texture);
    assetSystem.GetMetadataSerializer().Save(metadata, metadataPath);

    assetSystem.RegisterMetadata(metadata);

    assetSystem.LoadTexture(metadata);
}

bool TextureImporter::CanImport(const std::string& extension) const
{
    return extension == ".png" || extension == ".jpg";
}

bool TextureImporter::DecodeTexture(TextureAssetData& outTexture, const VirtualPath& sourcePath) const
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
