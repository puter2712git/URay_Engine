#include "TextureImporter.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Importer/ImportContext.h"
#include "Engine/Asset/Texture/Texture.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/Log.h"

#include <stb/stb_image.h>
#include <yaml-cpp/yaml.h>

namespace URay
{

TextureImporter::TextureImporter(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

TextureImporter::~TextureImporter() = default;

ImportResult TextureImporter::Import(const VirtualPath& path, ImportContext& context)
{
    ImportResult result = {};

    VirtualPath importMetaPath = VirtualPath(
        "Asset://" + path.GetRelativePath() + ".meta");
    VirtualPath importAssetPath = VirtualPath(
        "Asset://" + path.GetRelativePath() + ".asset");

    AssetMetadata metadata = {};
    Texture* texture = nullptr;

    if (!filesystem.Exists(importMetaPath))
    {
        metadata.uuid = UUID::Generate();
        metadata.type = AssetType::Texture;
        metadata.sourcePath = path;
        metadata.importPath = importAssetPath;

        YAML::Node metadataNode = metadata.Serialize();
        filesystem.WriteText(importMetaPath, YAML::Dump(metadataNode));
    }
    else
    {
        std::string metadataNodeString = filesystem.ReadText(importMetaPath);
        YAML::Node metadataNode = YAML::Load(metadataNodeString);
        metadata.Deserialize(metadataNode);
    }

    if (!filesystem.Exists(importAssetPath))
    {
        texture = LoadTexture(path, metadata, context);
    }

    result.entries.push_back(AssetEntry{
        .asset = texture,
        .metadata = metadata });

    return result;
}

bool TextureImporter::CanImport(const std::string& extension)
{
    if (extension == ".png" || extension == ".jpg")
    {
        return true;
    }

    return false;
}

Texture* TextureImporter::LoadTexture(const VirtualPath& path, const AssetMetadata& metadata, ImportContext& context)
{
    std::vector<uint8> fileBytes = filesystem.ReadBinary(path);

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

    std::vector<uint8> pixels(data, data + width * height * 4);

    AssetSystem& assetSystem = context.GetAssetSystem();
    AssetFactory& assetFactory = assetSystem.GetAssetFactory();

    Texture* newTexture = assetFactory.CreateTexture(metadata, width, height, channels, pixels);

    stbi_image_free(data);

    return newTexture;
}

} // namespace URay
