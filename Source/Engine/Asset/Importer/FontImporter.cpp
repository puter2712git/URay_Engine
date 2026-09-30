#include "FontImporter.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Font/Font.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/File/VirtualPath.h"

namespace URay
{

FontImporter::FontImporter() = default;

FontImporter::~FontImporter() = default;

void FontImporter::Import(const VirtualPath& sourcePath)
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

    std::vector<uint8> fileBytes = fileSystem.ReadBinary(metadata.sourcePath);

    std::unique_ptr<Font> font = std::make_unique<Font>(fileBytes);
    font->SetName(metadata.sourcePath.GetStem());
    font->SetHandle(metadata.handle);

    assetDatabase.Add(std::move(font));
}

AssetMetadata FontImporter::CreateMetadata(const VirtualPath& sourcePath) const
{
    AssetMetadata ret = {};

    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    ret.handle = AssetHandle::Generate();
    ret.type = AssetType::Font;
    ret.sourcePath = sourcePath;

    const VirtualPath importPath = assetSystem.GetImportAssetPath(sourcePath);
    ret.metadataPath = VirtualPath(importPath.ToString() + ".meta");
    ret.assetPath = VirtualPath(importPath.ToString() + ".asset");

    return ret;
}

bool FontImporter::CanImport(const std::string& extension) const
{
    return extension == ".ttf";
}

} // namespace URay
