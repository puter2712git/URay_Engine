#include "TextureImporter.h"

#include "Engine/Asset/AssetFactory.h"
#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Importer/ImportContext.h"
#include "Engine/Asset/Texture/Texture.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/Log.h"

#define STB_IMAGE_IMPLEMENTATION
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
    TextureCookData textureCookData = {};

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
        textureCookData = LoadTexture(path, metadata, context);

        std::vector<uint8> serializedCookData = serializer.Serialize(textureCookData);
        filesystem.WriteBinary(importAssetPath, serializedCookData);
    }
    else
    {
        std::vector<uint8> bytes = filesystem.ReadBinary(importAssetPath);
        textureCookData = serializer.Deserialize(bytes);
    }

    AssetSystem& assetSystem = context.GetAssetSystem();
    AssetFactory& assetFactory = assetSystem.GetAssetFactory();

    texture = assetFactory.CreateTexture(
        metadata,
        textureCookData.width,
        textureCookData.height,
        textureCookData.channels,
        textureCookData.pixels);

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

TextureCookData TextureImporter::LoadTexture(const VirtualPath& path, const AssetMetadata& metadata, ImportContext& context)
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
        return {};

    TextureCookData cookData = {};
    cookData.width = width;
    cookData.height = height;
    cookData.channels = channels;
    cookData.pixels = std::vector<uint8>(data, data + width * height * 4);

    stbi_image_free(data);

    return cookData;
}

} // namespace URay
