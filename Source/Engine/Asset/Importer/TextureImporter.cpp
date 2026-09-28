#include "TextureImporter.h"

#include "Engine/Asset/AssetDatabase.h"
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

void TextureImporter::Import(const VirtualPath& sourcePath)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    AssetMetadata metadata = {};

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    const VirtualPath metadataPath = VirtualPath(importPath.ToString() + ".meta");

    if (fileSystem.Exists(metadataPath))
    {
        const std::string fileText = fileSystem.ReadText(metadataPath);
        const YAML::Node node = YAML::Load(fileText);
        metadata.Deserialize(node);
    }
    else
    {
        metadata = CreateMetadata(sourcePath);
        const YAML::Node node = metadata.Serialize();
        fileSystem.WriteText(metadataPath, YAML::Dump(node));
    }

    int32 width, height, channels;
    std::vector<uint8> fileBytes = fileSystem.ReadBinary(metadata.sourcePath);

    stbi_uc* data = stbi_load_from_memory(
        fileBytes.data(),
        static_cast<int>(fileBytes.size()),
        &width, &height, &channels,
        STBI_rgb_alpha);
    if (!data)
        return;

    std::vector<uint8> pixels = std::vector<uint8>(data, data + width * height * 4);

    stbi_image_free(data);

    std::unique_ptr<Texture> texture = std::make_unique<Texture>(width, height, channels, pixels);
    texture->SetName(metadata.sourcePath.GetStem());
    texture->SetUUID(metadata.uuid);

    assetDatabase.Add(std::move(texture));
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

bool TextureImporter::CanImport(const std::string& extension) const
{
    return extension == ".png" || extension == ".jpg";
}

} // namespace URay
