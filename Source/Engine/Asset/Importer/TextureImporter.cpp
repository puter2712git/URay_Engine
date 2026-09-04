#include "TextureImporter.h"

#include "Engine/Asset/AssetMetadata.h"
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

std::vector<ImportResult> TextureImporter::Import(const VirtualPath& path)
{
    std::vector<ImportResult> results;

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

        YAML::Node metadataNode;
        metadataNode["UUID"] = metadata.uuid.ToString();
        metadataNode["Type"] = "Texture";
        metadataNode["SourcePath"] = metadata.sourcePath.ToString();
        metadataNode["ImportPath"] = metadata.importPath.ToString();

        filesystem.WriteText(importMetaPath, YAML::Dump(metadataNode));
    }
    else
    {
        std::string metadataNodeString = filesystem.ReadText(importMetaPath);
        YAML::Node metadataNode = YAML::Load(metadataNodeString);

        metadata.uuid = UUID::FromString(metadataNode["UUID"].as<std::string>());
        metadata.type = AssetType::Texture;
        metadata.sourcePath = VirtualPath(metadataNode["SourcePath"].as<std::string>());
        metadata.importPath = VirtualPath(metadataNode["ImportPath"].as<std::string>());
    }

    if (!filesystem.Exists(importAssetPath))
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

        std::vector<uint8> pixels(data, data + width * height * 4);

        texture = new Texture(
            path.ToString(),
            width,
            height,
            channels,
            pixels);
        texture->SetName(path.GetStem());
        texture->SetUUID(metadata.uuid);

        stbi_image_free(data);
    }

    results.push_back(ImportResult{
        .assetObject = texture,
        .metadata = metadata });

    return results;
}

} // namespace URay
