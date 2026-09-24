#include "TextureImporter.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <yaml-cpp/yaml.h>

namespace URay
{

TextureImporter::TextureImporter() = default;

TextureImporter::~TextureImporter() = default;

Asset* TextureImporter::Import(const VirtualPath& sourcePath)
{
    Asset* ret = nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetFactory& assetFactory = assetSystem.GetAssetFactory();
    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

    VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    VirtualPath metadataPath = VirtualPath(importPath.ToString() + ".meta");

    AssetMetadata metadata = {};

    if (!filesystem.Exists(metadataPath))
    {
        metadata = CreateMetadata(sourcePath);

        YAML::Node metadataNode = metadata.Serialize();
        filesystem.WriteText(metadataPath, YAML::Dump(metadataNode));
    }
    else
    {
        std::string metadataNodeString = filesystem.ReadText(metadataPath);
        YAML::Node metadataNode = YAML::Load(metadataNodeString);
        metadata.Deserialize(metadataNode);
    }

    ret = LoadTexture(sourcePath, metadata);

    return ret;
}

AssetMetadata TextureImporter::CreateMetadata(const VirtualPath& sourcePath) const
{
    AssetMetadata ret = {};

    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    ret.uuid = UUID::Generate();
    ret.type = AssetType::Texture;
    ret.sourcePath = sourcePath;

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    ret.metadataPath = VirtualPath(importPath.ToString() + ".meta");
    ret.assetPath = VirtualPath(importPath.ToString() + ".asset");

    return ret;
}

std::vector<UUID> TextureImporter::CollectDependencies(const VirtualPath& sourcePath) const
{
    return {};
}

bool TextureImporter::CanImport(const std::string& extension) const
{
    return extension == ".png" || extension == ".jpg";
}

Texture* TextureImporter::LoadTexture(const VirtualPath& sourcePath, const AssetMetadata& metadata) const
{
    Texture* ret = nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetFactory& factory = assetSystem.GetAssetFactory();
    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

    std::vector<uint8> fileBytes = filesystem.ReadBinary(sourcePath);

    int width, height, channels;
    stbi_uc* data = stbi_load_from_memory(
        fileBytes.data(),
        static_cast<int>(fileBytes.size()),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);

    if (!data)
        return nullptr;

    std::vector<uint8> pixels = std::vector<uint8>(data, data + width * height * 4);
    ret = factory.CreateTexture(metadata, width, height, channels, pixels);

    stbi_image_free(data);

    return ret;
}

} // namespace URay
