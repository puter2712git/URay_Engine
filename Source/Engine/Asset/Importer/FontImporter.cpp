#include "FontImporter.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Font/Font.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>
#include <Engine/Engine.h>

namespace URay
{

FontImporter::FontImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

FontImporter::~FontImporter() = default;

ImportResult FontImporter::Import(const VirtualPath& sourcePath)
{
    const VirtualPath metadataPath = assetSystem.GetMetadataPath(sourcePath);
    const AssetMetadata metadata = LoadOrCreateMetadata(sourcePath, metadataPath, Font::StaticClass());

    ImportResult result = ImportResult::Success();
    result.metadatas.push_back(metadata);
    return result;

    // std::vector<uint8> fileBytes = fileSystem.ReadBinary(metadata.sourcePath);

    // std::unique_ptr<Font> font = std::make_unique<Font>(fileBytes);
    // font->SetName(metadata.sourcePath.GetStem());
    // font->SetHandle(metadata.handle);

    // assetDatabase.Add(std::move(font));
}

bool FontImporter::CanImport(const std::string& extension) const
{
    return extension == ".ttf";
}

} // namespace URay
