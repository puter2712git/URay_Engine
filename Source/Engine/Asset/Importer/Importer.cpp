#include "Importer.h"

#include <Core/File/VirtualFileSystem.h>

namespace URay
{

Importer::Importer(AssetSystem& assetSystem, VirtualFileSystem& fileSystem)
    : assetSystem(assetSystem), fileSystem(fileSystem) {}

AssetMetadata Importer::LoadOrCreateMetadata(const VirtualPath& sourcePath, const VirtualPath& metadataPath, AssetType assetType) const
{
    const std::string metadataText = fileSystem.ReadText(metadataPath);

    AssetMetadata metadata;

    if (metadataText.empty())
    {
        metadata.handle = AssetHandle::Generate();
        metadata.type = assetType;
        metadata.sourcePath = sourcePath;
        metadata.assetPath = metadataPath.RemoveExtension();
    }
    else
    {
        YAML::Node node = YAML::Load(metadataText);
        metadata.Deserialize(node);
    }

    return metadata;
}

} // namespace URay
