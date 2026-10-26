#include "FontImporter.h"
#include <Core/Type/TypeAlias.h>

#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Font/Font.h>
#include <Engine/Asset/Font/FontData.h>
#include <Engine/Asset/Font/FontSerializer.h>
#include <Engine/Asset/Importer/ImportRequest.h>
#include <Engine/Asset/Metadata/AssetMetadata.h>

namespace URay
{

FontImporter::FontImporter(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : Importer(assetSystem, fileSystem) {}

FontImporter::~FontImporter() = default;

ImportResult FontImporter::Import(const ImportRequest& request)
{
    const VirtualPath assetPath = request.outputDirectory.Join(request.sourcePath.GetStem() + ".urfont");
    const VirtualPath metadataPath = assetSystem.GetMetadataPath(assetPath);

    FontData fontData;

    if (!DecodeFont(fontData, request.sourcePath))
        return ImportResult::Failure("Failed to decode font: " + request.sourcePath.ToString());

    FontSerializer& serializer = assetSystem.GetFontSerializer();
    if (!serializer.Save(fontData, assetPath))
        return ImportResult::Failure("Failed to save font: " + assetPath.ToString());

    const AssetMetadata metadata = LoadOrCreateMetadata(request.sourcePath, metadataPath, Font::StaticClass());

    ImportResult result = ImportResult::Success();
    result.metadatas.push_back(metadata);
    return result;
}

bool FontImporter::CanImport(const String& extension) const
{
    return extension == ".ttf";
}

bool FontImporter::DecodeFont(FontData& outFont, const VirtualPath& sourcePath) const
{
    std::vector<uint8> fileBytes = fileSystem.ReadBinary(sourcePath);
    if (fileBytes.empty())
        return false;

    outFont = {};
    outFont.bytes = std::move(fileBytes);

    return true;
}

} // namespace URay
